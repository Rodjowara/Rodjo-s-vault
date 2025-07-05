#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>

#define SALT_SIZE 16
#define IV_SIZE 16
#define KEY_SIZE 32
#define ITERATIONS 100000

using namespace std;

vector<unsigned char> key_derivation(const string &master, const vector<unsigned char> &salt){

    vector<unsigned char> key(KEY_SIZE);
    PKCS5_PBKDF2_HMAC(master.c_str(), master.size(), salt.data(), SALT_SIZE, ITERATIONS, EVP_sha256(), KEY_SIZE, key.data());

    return key;
}

vector<unsigned char> data_encription(const string &master, const string &data){

    vector<unsigned char> salt(SALT_SIZE);
    vector<unsigned char> iv(IV_SIZE);
    RAND_bytes(salt.data(), SALT_SIZE);
    RAND_bytes(iv.data(), IV_SIZE);

    vector key = key_derivation(master, salt);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    vector<unsigned char> cipher(data.size() + AES_BLOCK_SIZE);
    int len = 0;
    int cipher_length = 0;

    EVP_EncryptUpdate(ctx, cipher.data(), &len, reinterpret_cast<const unsigned char*>(data.c_str()), data.size());
    cipher_length += len;
    EVP_EncryptFinal_ex(ctx, cipher.data() + len, &len);
    cipher_length += len;
    EVP_CIPHER_CTX_free(ctx);

    cipher.resize(cipher_length);

    vector<unsigned char> encrypted_data;
   
    encrypted_data.insert(encrypted_data.end(), salt.begin(), salt.end());
    encrypted_data.insert(encrypted_data.end(), iv.begin(), iv.end());
    encrypted_data.insert(encrypted_data.end(), cipher.begin(), cipher.end());

    return encrypted_data;
}

string data_decription(const string &master, const vector<unsigned char> encrypted_data){

    if(encrypted_data.size() < IV_SIZE + SALT_SIZE){
        return "";
    }

    vector<unsigned char> salt(encrypted_data.begin(), encrypted_data.begin() + SALT_SIZE);
    vector<unsigned char> iv(encrypted_data.begin() + SALT_SIZE, encrypted_data.begin() + IV_SIZE + SALT_SIZE);
    vector<unsigned char> cipher_text(encrypted_data.begin() + SALT_SIZE + IV_SIZE, encrypted_data.end());

    vector<unsigned char> key = key_derivation(master, salt);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    vector<unsigned char> decrypted_text(cipher_text.size() + AES_BLOCK_SIZE);
    int length = 0;
    int plaintext_length = 0;

    if(!EVP_DecryptUpdate(ctx, decrypted_text.data(), &length, cipher_text.data(), cipher_text.size())){


        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    plaintext_length += length;

    if(!EVP_DecryptFinal_ex(ctx, decrypted_text.data() + length, &length)){

        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    plaintext_length += length;
    EVP_CIPHER_CTX_free(ctx);

    decrypted_text.resize(plaintext_length);

    return string(decrypted_text.begin(), decrypted_text.end());

}

void save_encrypted_passwords(const vector<unsigned char> &encrypted_data, const string &filename){
    ofstream file(filename, ios::binary);
    file.write(reinterpret_cast<const char *>(encrypted_data.data()), encrypted_data.size());
    file.close();
}

vector<unsigned char> load_encrypted_passwords(const string &filename){
    ifstream file(filename, ios::binary);
    return vector<unsigned char>((istreambuf_iterator<char>(file)), istreambuf_iterator<char>()); 
}

int main(int argc, char* argv[]){

    if(argc < 3){
        cout << "Neispravan unos, nedovoljno argumenata" << endl;
        return 1;
    }

    string filename = "database.txt";

    if(strcmp(argv[1], "init") == 0){

        string master = argv[2];
        vector<unsigned char> encrypted_data = data_encription(master, " ");
        save_encrypted_passwords(encrypted_data, filename);
        cout << "Password manager initialised." << endl;

    }else if(strcmp(argv[1], "put") == 0 && argc == 5){

        string master = argv[2];
        string page = argv[3];
        string password = argv[4];

        vector<unsigned char> encrypted_data = load_encrypted_passwords(filename);
        string data  = data_decription(master, encrypted_data);

        if(data.empty()){
            cout << "Master password incorrect or integrity check failed." << endl;
            return 1;
        }

        string new_data = data + "{ \"" + page + "\": \"" + password + "\" }";
        encrypted_data = data_encription(master, new_data);
        
        save_encrypted_passwords(encrypted_data, filename);

        cout << "Stored password for " << page << endl;

    }else if(strcmp(argv[1], "get") == 0 && argc == 4){
        
        string master = argv[2];
        string page = argv[3];

        vector<unsigned char> encrypted_data = load_encrypted_passwords(filename);
        string data = data_decription(master, encrypted_data);

        if(data.empty()){
            cout << "Master password incorrect or integrity check failed." << endl;
            return 1;
        }

        size_t position = data.find(page);
        if(position != string::npos){
            size_t start = data.find(":", position) + 2;
            size_t end = data.find("}", start);
            string saved_password = data.substr(start, end - start);
            std::cout << "Password for " << page << " is: " << saved_password << endl;
        }else{
            cout << "No password found for " << page << endl;
        }

    }else{
        cout << "Neispravan unos, nepostojeća komanda" << endl;
    }

    return 0;
}