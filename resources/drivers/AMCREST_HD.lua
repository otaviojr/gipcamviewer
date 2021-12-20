local http = require "http.request"
local socket = require("socket")
local tcp = assert(socket.tcp())

function driver_get_base_url()
  print("AMCREST_HD: get base url for " .. camera_name .. " camera")

  local local_protocol = "rtsp://"
  local remote_protocol = "rtsp://"

  --[[Test local URI]]
  local err = tcp:connect(camera_local_url, camera_local_port);
  if (nil ~= err) then
    print("FOSCAM_FI8918W: Local URI success..")
    return "", local_protocol .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_media_port
  end

  --[[Test remote URI]]
  local err = tcp:connect(camera_remote_url, camera_remote_port);
  if (nil ~= err) then
    print("AMCREST HD: Remote URI success..")
    return "",remote_protocol .. camera_username .. ":" .. camera_password .. "@" .. camera_remote_url .. ":" .. camera_remote_media_port
  end

  return nil, nil
end

function driver_get_stream_url(base_url, base_media_url)
  print("AMCREST HD: get stream url for " .. camera_name .. " camera")
  return base_url .. "/cam/realmonitor?channel=1&subtype=0"
end

function driver_move_up(base_url, base_media_url)
  return false
end

function driver_move_down(base_url, base_media_url)
  return false
end

function driver_move_left(base_url, base_media_url)
  return false
end

function driver_move_right(base_url, base_media_url)
  return false
end

function driver_is_flipped(base_url, base_media_url)
  return false
end

function driver_is_mirrored(base_url, base_media_url)
  return false
end
