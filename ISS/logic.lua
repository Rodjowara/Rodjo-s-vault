-- Create a persistence object for the zoom level
prs_zoom     = persist_add("zoom", 12)
prs_hdg_mode = persist_add("hdg_mode", 1)

-- General vars --
local gps_zoom = persist_get(prs_zoom)

-- Button functions --
function new_zoomplus()
    gps_zoom = var_cap(gps_zoom + 1, 7, 14)
    persist_put(prs_zoom, gps_zoom)
    map_zoom(navmap, gps_zoom)
end

function new_zoommin()
    gps_zoom = var_cap(gps_zoom - 1, 7, 14)
    persist_put(prs_zoom, gps_zoom)
    map_zoom(navmap, gps_zoom)
end

function new_home()
    gps_zoom = 12
    persist_put(prs_zoom, gps_zoom)
    map_zoom(navmap, gps_zoom)
end

function new_hdg_select()
    persist_put(prs_hdg_mode, (persist_get(prs_hdg_mode) + 1) % 2)
    request_callback(new_position)
end

-- Load and display map and images --
img_add_fullscreen("GPSback.png")
img_add("zoomgps.png", 531, 302, 32, 78)
prop_map_type = user_prop_add_enum("Map type", "OSM_STANDARD,OSM_CYCLE,OSM_TRANSPORT,OSM_HUMANITARIAN", "OSM_STANDARD", "Choose which type of map to show.")
prop_temperature = user_prop_add_enum("Temperature", "Celsius,Fahrenheit", "Celsius", "Choose the unit for the temperature.")

navmap = map_add(79, 120, 415, 351, user_prop_get(prop_map_type), gps_zoom)

map_add_nav_img_layer(navmap, "AIRPORT", "airport.png", -13, -13, 26, 26)
map_add_nav_txt_layer(navmap, "AIRPORT", "ICAO", "size:14px; color:#2F4F4F; font:arimo_bold.ttf; halign:center;", -40, -30, 80, 40)
img_north_arrow = img_add("northarrow.png", 90, 130, 25, 25)
airplane_icon = img_add("airplane.png", 275, 285, 25, 25)

-- Create a canvas overlay for the map
local flight_path_canvas = canvas_add(80, 122, 415, 351, function()
    -- Initial drawing function; updated dynamically
end)

-- Hardcoded flight path starting at Seattle Tacoma Intl (KSEA)
local waypoints = {
    {47.449, -122.309}, -- KSEA
    {47.53, -122.302}   -- KBFI
}

--local waypoints = {}
--local loaded = 0

------------funkcija za dohvaćanje waypointova direktno iz x-planea-------------
-----problem: ne znamo gdje x-plane sprema waypointove--------------------------
--function load_waypoints(latitudes, longitudes)
    --waypoints = {}
    
    -- Iterate through the latitudes and longitudes arrays
    --for i = 1, #latitudes, 1 do
        --if latitudes[i] ~= 0 and longitudes[i] ~= 0 then  -- Ignore invalid coordinates
            --table.insert(waypoints, {
                --latitude = latitudes[i],
                --longitude = longitudes[i]
            --})
        --end
--end

    -- Print confirmation and contents for debugging
    --print("Waypoints loaded:")
    --for i, waypoint in ipairs(waypoints) do
        --print(string.format("Waypoint %d: Latitude %.6f, Longitude %.6f", i, waypoint.latitude, waypoint.longitude))
    --end
--end



-------funkcija za dohvaćanje waypointova iz .fms datoteke----
--function parse_fms_file(file_path)
    --waypoints = {} -- Clear previous data

    -- Open the file in read mode
    --local file = io.open(file_path, "r")
    --if not file then
        --print("Error: Unable to open file " .. file_path)
        --return false
    --end

    -- Read the header (first line)
    --local header = file:read("*line")
    --if not header:match("^I") then
        --print("Error: Invalid .fms file format.")
        --file:close()
        --return false
    --end

    -- Skip the second line (usually a version number or metadata)
    --file:read("*line")

    -- Read each waypoint line
    --for line in file:lines() do
        --local fields = {}
        --for field in line:gmatch("[^%s]+") do
            --table.insert(fields, field)
        --end

         --Each line contains: waypoint type, altitude, latitude, longitude, identifier
        --if #fields >= 5 then
            --local waypoint = {
                --latitude = tonumber(fields[3]),  -- Latitude in decimal degrees
                --longitude = tonumber(fields[4]), -- Longitude in decimal degrees
            --}
           --table.insert(waypoints, waypoint)
           
        --end
    --end

    --file:close()
    --print("Successfully loaded " .. #waypoints .. " waypoints from " .. file_path)
    --loaded = 1
    --return true
--end

-- ovdje se navede file path za fms datoteku
--local file_path = "desktop/waypoints.fms"

-- Function to calculate map bounds based on zoom and center
function calculate_map_bounds(center_lat, center_lon, zoom)
    -- These constants determine the approximate map range per zoom level
    local lat_range_per_zoom = {90, 45, 22.5, 11.25, 5.625, 2.8125, 1.40625, 0.703125, 0.3515625, 0.17578125, 0.087890625, 0.0439453125, 0.02197265625, 0.010986328125}
    local lon_range_per_zoom = {180, 90, 45, 22.5, 11.25, 5.625, 2.8125, 1.40625, 0.703125, 0.3515625, 0.17578125, 0.087890625, 0.0439453125, 0.02197265625}

    local lat_range = lat_range_per_zoom[zoom]
    local lon_range = lon_range_per_zoom[zoom]

    -- Calculate bounds
    local min_lat = center_lat - lat_range / 2
    local max_lat = center_lat + lat_range / 2
    local min_lon = center_lon - lon_range / 2
    local max_lon = center_lon + lon_range / 2

    return {min_lat = min_lat, max_lat = max_lat, min_lon = min_lon, max_lon = max_lon}
end

-- Replace map_get_bounds with calculate_map_bounds
function geo_to_screen(lat, lon, bounds)
    local map_width, map_height = 415, 351
    local x = (lon - bounds.min_lon) / (bounds.max_lon - bounds.min_lon) * map_width
    local y = (1 - (lat - bounds.min_lat) / (bounds.max_lat - bounds.min_lat)) * map_height
    return x, y
end

--funkcija za iscrtavanje flight patha
function update_flight_path(center_lat, center_lon)
    -- Calculate bounds based on current map center and zoom level
    local bounds = calculate_map_bounds(center_lat, center_lon, gps_zoom)

    -- Redraw flight path on the canvas
    canvas_draw(flight_path_canvas, function()
    if #waypoints > 1 then
        for i = 1, #waypoints - 1 do
            local lat1, lon1 = waypoints[i][1], waypoints[i][2]
            local lat2, lon2 = waypoints[i + 1][1], waypoints[i + 1][2]

            -- Convert geo coordinates to screen coordinates
            local x1, y1 = geo_to_screen(lat1, lon1, bounds)
            local x2, y2 = geo_to_screen(lat2, lon2, bounds)

            -- Debugging coordinates
            print(string.format("Drawing line: (%.2f, %.2f) to (%.2f, %.2f)", x1, y1, x2, y2))

            -- Draw the line
            _move_to(x1, y1)
            _line_to(x2, y2)
            _stroke("red", 6) -- Line from (x1, y1) to (x2, y2) in red with thickness 3
            end
        end
    end)
end

function new_position(latitude, longitude, heading, groundspeed, timer)
    -- Set position on the map
    map_goto(navmap, latitude, longitude)

    -- Rotate airplane and map based on heading mode
    if persist_get(prs_hdg_mode) == 0 then -- North up
        rotate(airplane_icon, heading, "LOG", 0.05)
        rotate(navmap, 0, "LOG", 0.05)
        rotate(img_north_arrow, 0, "LOG", 0.05)
    elseif persist_get(prs_hdg_mode) == 1 then -- Heading up
        rotate(airplane_icon, 0, "LOG", 0.05)
        rotate(navmap, heading * -1, "LOG", 0.05)
        rotate(img_north_arrow, 360 - heading, "LOG", 0.05)
    end

    -- Update flight path with the new center position
    update_flight_path(latitude, longitude)
    
    --ovo je if statement ako waypoints čitamo iz .fms filea
    --ako smo ga već pročitali (vraijabla loaded, funkcija za čitanje datoteke ju stavlja u 1)
    --onda ga ne moramo opet čitati
    --if loaded == 0 then
        --parse_fms_file(file_path)
    --end    
end


-- Switches, buttons and dials --
button_zoomplus = button_add("zoomplus.png", "zoompluspr.png", 533, 304, 28, 28, new_zoomplus)
button_zoommin  = button_add("zoommin.png", "zoomminpres.png", 533, 350, 28, 28, new_zoommin)
button_home     = button_add("homebutton.png", "homebuttonpr.png", 527, 391, 40, 35, new_home)
button_hdg_slct = button_add(nil, nil, 80, 120, 45, 45, new_hdg_select)

-- Data subscription --
xpl_dataref_subscribe("sim/flightmodel/position/latitude", "DOUBLE",
                      "sim/flightmodel/position/longitude", "DOUBLE", 
                      "sim/flightmodel/position/mag_psi", "FLOAT",
                      "sim/flightmodel/position/groundspeed", "FLOAT", 
                      "sim/time/total_flight_time_sec", "FLOAT", new_position)
                      
 --xpl_dataref_subscribe("sim/cockpit2/radios/indicators/gps_waypoint_lat", "FLOAT[10]",   
                      -- "sim/cockpit2/radios/indicators/gps_waypoint_lon", "FLOAT[10]", load_waypoints)
