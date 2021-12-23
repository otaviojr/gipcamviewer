package.path = package.path .. ";" .. drivers_dir .. "/?.lua"
local http_digest = require("http_digest")
local socket = require("socket")
local tcp = assert(socket.tcp())

function driver_get_base_url()
  print("AMCREST_HD: get base/media url for " .. camera_name .. " camera")

  local local_protocol = "rtsp://"
  local remote_protocol = "rtsp://"

  --[[Test local URI]]
  local err = tcp:connect(camera_local_url, camera_local_port);
  if (nil ~= err) then
    print("FOSCAM_FI8918W: Local URI success..")
    return local_protocol .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_media_port, "http://" .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_port
  end

  --[[Test remote URI]]
  local err = tcp:connect(camera_remote_url, camera_remote_port);
  if (nil ~= err) then
    print("AMCREST HD: Remote URI success..")
    return remote_protocol .. camera_username .. ":" .. camera_password .. "@" .. camera_remote_url .. ":" .. camera_remote_media_port, "http://" .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_port
  end

  return nil, nil
end

function driver_get_stream_url(base_url, base_media_url)
  print("AMCREST HD: get stream url for " .. camera_name .. " camera")
  return base_media_url .. "/cam/realmonitor?channel=1&subtype=0"
end

function dump_error(body, code, headers)
  print("AMCREST HD: status code: " .. code)
  print("AMCREST HD: body: " .. body)
end

function driver_stop_motion(base_url, base_media_url, action)
  print("AMCREST_HD: stop motion for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/ptz.cgi?action=stop&channel=0&code=" .. action .. "&arg1=0&arg2=1&arg3=0&arg4=0"
  print("AMCREST_HD: Stop motion URI: " .. uri)
  local b, c, h = http_digest.request(uri)
  if c == 200 and b ~= nil then
    return true
  else
    dump_error(b, c, h)
  end

  print("AMCREST_HD:  stop motion failed")
  return false
end

function driver_move_up(base_url, base_media_url)
  print("AMCREST_HD: move up for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/ptz.cgi?action=start&channel=0&code=Up&arg1=0&arg2=1&arg3=0&arg4=0"
  print("AMCREST_HD: Move Up URI: " .. uri)
  local b, c, h = http_digest.request {
    url = uri,
    method = "GET"
  }
  if c == 200 and b ~= nil then
    sleep(50)
    driver_stop_motion(base_url, base_media_url, "Up")
    return true
  else
    dump_error(b, c, h)
  end

  print("AMCREST_HD:  move up failed")
  return false
end

function driver_move_down(base_url, base_media_url)
  print("AMCREST_HD: move down for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/ptz.cgi?action=start&channel=0&code=Down&arg1=0&arg2=1&arg3=0&arg4=0"
  print("AMCREST_HD: Move Down URI: " .. uri)
  local b, c, h = http_digest.request {
    url = uri,
    method = "GET"
  }
  if c == 200 and b ~= nil then
    sleep(50)
    driver_stop_motion(base_url, base_media_url, "Down")
    return true
  else
    dump_error(b, c, h)
  end
  print("AMCREST_HD:  move down failed")
  return false
end

function driver_move_left(base_url, base_media_url)
  print("AMCREST_HD: move left for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/ptz.cgi?action=start&channel=0&code=Left&arg1=0&arg2=1&arg3=0&arg4=0"
  print("AMCREST_HD: Move Left URI: " .. uri)
  local b, c, h = http_digest.request {
    url = uri,
    method = "GET"
  }
  if c == 200 and b ~= nil then
    sleep(50)
    driver_stop_motion(base_url, base_media_url, "Left")
    return true
  else
    dump_error(b, c, h)
  end
  print("AMCREST_HD:  move left failed")
  return false
end

function driver_move_right(base_url, base_media_url)
  print("AMCREST_HD: move right for " .. camera_name .. " camera")
  local uri = base_url .. "/cgi-bin/ptz.cgi?action=start&channel=0&code=Right&arg1=0&arg2=1&arg3=0&arg4=0"
  print("AMCREST_HD: Move Right URI: " .. uri)
  local b, c, h = http_digest.request {
    url = uri,
    method = "GET"
  }
  if c == 200 and b ~= nil then
    sleep(50)
    driver_stop_motion(base_url, base_media_url, "Right")
    return true
  else
    dump_error(b, c, h)
  end
  print("AMCREST_HD:  move right failed")
  return false
end

function driver_is_flipped(base_url, base_media_url)
  return false
end

function driver_is_mirrored(base_url, base_media_url)
  return false
end
