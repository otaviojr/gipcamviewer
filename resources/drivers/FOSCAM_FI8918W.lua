local http = require "http.request"

function driver_get_base_url()
  print("LUA: get base url for " .. camera_name .. " camera")

  --[[Test local URI]]
  local uri = "http://" .. camera_local_url .. ":" .. camera_local_port .. "/get_status.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Testing local URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("LUA: Local URI success..")
      return "", "http://" .. camera_local_url .. ":" .. camera_local_port
    else
      print("LUA: Local URI failed..")
    end
  else
    print("LUA: Local URI failed..")
  end

  --[[Test remote URI]]
  local uri = "http://" .. camera_remote_url .. ":" .. camera_remote_port .. "/get_status.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Testing remote URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("LUA: Remote URI success..")
      return "","http://" .. camera_remote_url .. ":" .. camera_remote_port
    else
      print("LUA: Remote URI failed..")
    end
  else
    print("LUA: Remote URI failed..")
  end
  return nil, nil
end

function driver_get_stream_url(base_url, base_media_url)
  print("LUA: get stream url for " .. camera_name .. " camera")
  return base_url .. "/videostream.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
end

function driver_move_up(base_url, base_media_url)
  print("LUA: move up for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=0&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Move Up URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end

  print("LUA:  move up failed")
  return false
end

function driver_move_down(base_url, base_media_url)
  print("LUA: move down for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=2&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Move Down URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("LUA:  move down failed")
  return false
end

function driver_move_left(base_url, base_media_url)
  print("LUA: move left for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=4&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Move Left URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("LUA:  move left failed")
  return false
end

function driver_move_right(base_url, base_media_url)
  print("LUA: move right for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=6&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Move Right URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("LUA:  move right failed")
  return false
end

function driver_is_flipped(base_url, base_media_url)
  print("LUA: checking flipped for " .. camera_name .. " camera")
  local uri = base_url .. "/get_camera_params.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Flip Checked URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      local value = string.match(string.match(body, 'flip=%d'),"%d")
      print("LUA: flip value = " .. value)
      if value == "1" or value == "3" then
        return true
      end
    end
  end

  print("LUA:  flip check failed")
  return false
end

function driver_is_mirrored(base_url, base_media_url)
  print("LUA: checking flipped for " .. camera_name .. " camera")
  local uri = base_url .. "/get_camera_params.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("LUA: Flip Checked URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      local value = string.match(string.match(body, 'flip=%d'),"%d")
      print("LUA: mirror value = " .. value)
      if value == "2" or value == "3" then
        return true
      end
    end
  end

  print("LUA:  flip check failed")
  return false
end
