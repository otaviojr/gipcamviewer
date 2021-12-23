local http = require "http.request"

function driver_get_base_url()
  print("FOSCAM_FI8918W: get base url for " .. camera_name .. " camera")

  local local_protocol = "http://"
  if camera_local_https == true then
    local_protocol = "https://"
  end

  local remote_protocol = "http://"
  if camera_remote_https == true then
    remote_protocol = "https://"
  end

  --[[Test local URI]]
  local uri = local_protocol .. camera_local_url .. ":" .. camera_local_port .. "/get_status.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Testing local URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("FOSCAM_FI8918W: Local URI success..")
      return "", local_protocol .. camera_local_url .. ":" .. camera_local_port
    else
      print("FOSCAM_FI8918W: Local URI failed..")
    end
  else
    print("FOSCAM_FI8918W: Local URI failed..")
  end

  --[[Test remote URI]]
  local uri = remote_protocol .. camera_remote_url .. ":" .. camera_remote_port .. "/get_status.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Testing remote URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("FOSCAM_FI8918W: Remote URI success..")
      return "",remote_protocol .. camera_remote_url .. ":" .. camera_remote_port
    else
      print("FOSCAM_FI8918W: Remote URI failed..")
    end
  else
    print("FOSCAM_FI8918W: Remote URI failed..")
  end
  return nil, nil
end

function driver_get_stream_url(base_url, base_media_url)
  print("FOSCAM_FI8918W: get stream url for " .. camera_name .. " camera")
  return base_url .. "/videostream.asf?user=" .. camera_username .. "&pwd=" .. camera_password
end

function driver_move_up(base_url, base_media_url)
  print("FOSCAM_FI8918W: move up for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=0&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Move Up URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end

  print("FOSCAM_FI8918W:  move up failed")
  return false
end

function driver_move_down(base_url, base_media_url)
  print("FOSCAM_FI8918W: move down for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=2&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Move Down URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("FOSCAM_FI8918W:  move down failed")
  return false
end

function driver_move_left(base_url, base_media_url)
  print("FOSCAM_FI8918W: move left for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=4&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Move Left URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("FOSCAM_FI8918W:  move left failed")
  return false
end

function driver_move_right(base_url, base_media_url)
  print("FOSCAM_FI8918W: move right for " .. camera_name .. " camera")
  local uri = base_url .. "/decoder_control.cgi?command=6&onestep=1&user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Move Right URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("FOSCAM_FI8918W:  move right failed")
  return false
end

function driver_is_flipped(base_url, base_media_url)
  print("FOSCAM_FI8918W: checking flipped for " .. camera_name .. " camera")
  local uri = base_url .. "/get_camera_params.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Flip Checked URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      local value = string.match(string.match(body, 'flip=%d'),"%d")
      print("FOSCAM_FI8918W: flip value = " .. value)
      if value == "1" or value == "3" then
        return true
      else
        return false
      end
    end
  end

  print("FOSCAM_FI8918W:  flip check failed")
  return false
end

function driver_is_mirrored(base_url, base_media_url)
  print("FOSCAM_FI8918W: checking mirrored for " .. camera_name .. " camera")
  local uri = base_url .. "/get_camera_params.cgi?user=" .. camera_username .. "&pwd=" .. camera_password
  print("FOSCAM_FI8918W: Mirror Checked URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      local value = string.match(string.match(body, 'flip=%d'),"%d")
      print("FOSCAM_FI8918W: mirror value = " .. value)
      if value == "2" or value == "3" then
        return true
      else
        return false
      end
    end
  end

  print("FOSCAM_FI8918W:  mirror check failed")
  return false
end
