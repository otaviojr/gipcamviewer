local http = require "http.request"

function driver_get_base_url()
  print("FOSCAM_FI9821P: get base url for " .. camera_name .. " camera")

  --[[Test local URI]]
  local uri = "http://" .. camera_local_url .. ":" .. camera_local_port .. "/cgi-bin/CGIProxy.fcgi?usr=" .. camera_username .. "&pwd=" .. camera_password .. "&cmd=setSubVideoStreamType&streamType=1"
  print("FOSCAM_FI9821P: Testing local URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("FOSCAM_FI9821P: Local URI success..")
      return "rtsp://" .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_media_port, "http://" .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_port
    else
      print("FOSCAM_FI9821P: Local URI failed..")
    end
  else
    print("FOSCAM_FI9821P: Local URI failed..")
  end

  --[[Test remote URI]]
  local uri = "http://" .. camera_remote_url .. ":" .. camera_remote_port .. "/cgi-bin/CGIProxy.fcgi?usr=" .. camera_username .. "&pwd=" .. camera_password .. "&cmd=setSubVideoStreamType&streamType=1"
  print("FOSCAM_FI9821P: Testing remote URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("FOSCAM_FI9821P: Remote URI success..")
      return "rtsp://" .. camera_username .. ":" .. camera_password .. "@" .. camera_remote_url .. ":" .. camera_remote_media_port, "http://" .. camera_username .. ":" .. camera_password .. "@" .. camera_remote_url .. ":" .. camera_remote_port
    else
      print("FOSCAM_FI9821P: Remote URI failed..")
    end
  else
    print("FOSCAM_FI9821P: Remote URI failed..")
  end
  return nil,nil
end

function driver_get_stream_url(base_url, base_media_url)
  print("FOSCAM_FI9821P: get stream url for " .. camera_name .. " camera")
  return base_media_url .. "/videoMain"
end

function driver_is_flipped(base_url, base_media_url)
  print("FOSCAM_FI9821P: checking flipped for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=getMirrorAndFlipSetting&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Flip Checked URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      local value = string.match(string.match(body, '<isFlip>%d</isFlip>'),"%d")
      print("FOSCAM_FI9821P: flip value = " .. value)
      if value == "1" then
        return true
      else
        return false
      end
    end
  end

  print("FOSCAM_FI9821P:  flip check failed")
  return false
end

function driver_is_mirrored(base_url, base_media_url)
  print("FOSCAM_FI9821P: checking mirrored for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=getMirrorAndFlipSetting&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Mirror Checked URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      local value = string.match(string.match(body, '<isMirror>%d</isMirror>'),"%d")
      print("FOSCAM_FI9821P: mirror value = " .. value)
      if value == "1" then
        return true
      else
        return false
      end
    end
  end

  print("FOSCAM_FI9821P:  mirror check failed")
  return false
end

function _driver_stop_motion(base_url, base_media_url)
  print("FOSCAM_FI9821P: stopping for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=ptzStopRun&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Stop Motion URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      return true
    end
  end
  print("FOSCAM_FI9821P: stop motion failed")
  return false
end

function driver_move_up(base_url, base_media_url)
  print("FOSCAM_FI9821P: move up for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=ptzMoveUp&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Move Up URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      sleep(500)
      _driver_stop_motion(base_url, base_media_url)
      return true
    end
  end

  print("FOSCAM_FI9821P:  move up failed")
  return false
end

function driver_move_down(base_url, base_media_url)
  print("FOSCAM_FI9821P: move down for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=ptzMoveDown&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Move Down URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      sleep(500)
      _driver_stop_motion(base_url, base_media_url)
      return true
    end
  end
  print("FOSCAM_FI9821P:  move down failed")
  return false
end

function driver_move_left(base_url, base_media_url)
  print("FOSCAM_FI9821P: move left for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=ptzMoveLeft&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Move Left URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      sleep(500)
      _driver_stop_motion(base_url, base_media_url)
      return true
    end
  end
  print("FOSCAM_FI9821P:  move left failed")
  return false
end

function driver_move_right(base_url, base_media_url)
  print("FOSCAM_FI9821P: move right for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/CGIProxy.fcgi?cmd=ptzMoveRight&usr="..camera_username.."&pwd="..camera_password
  print("FOSCAM_FI9821P: Move Right URI: " .. uri)
  local req = http.new_from_uri(uri)
  req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      sleep(500)
      _driver_stop_motion(base_url, base_media_url)
      return true
    end
  end
  print("FOSCAM_FI9821P:  move right failed")
  return false
end
