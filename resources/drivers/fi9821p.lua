local http = require "http.request"

function driver_get_stream_url()
  print("LUA: get stream url for " .. camera_name .. " camera")

  --[[Test local URI]]
  local uri = "http://" .. camera_local_url .. ":" .. camera_local_port .. "/cgi-bin/CGIProxy.fcgi?usr=" .. camera_username .. "&pwd=" .. camera_password .. "&cmd=setSubVideoStreamType&streamType=1"
  print("LUA: Testing local URI: " .. uri)
  local req = http.new_from_uri(uri)
	req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("LUA: Local URI success..")
      return "rtsp://" .. camera_username .. ":" .. camera_password .. "@" .. camera_local_url .. ":" .. camera_local_media_port .. "/videoMain"
    else
      print("LUA: Local URI failed..")
    end
  else
    print("LUA: Local URI failed..")
  end

  --[[Test remote URI]]
  local uri = "http://" .. camera_remote_url .. ":" .. camera_remote_port .. "/cgi-bin/CGIProxy.fcgi?usr=" .. camera_username .. "&pwd=" .. camera_password .. "&cmd=setSubVideoStreamType&streamType=1"
  print("LUA: Testing remote URI: " .. uri)
  local req = http.new_from_uri(uri)
	req.headers:upsert(":method", "GET")
  local headers, stream = req:go(2)
  if headers ~= nil and headers:get ":status" == "200" then
    local body, err = stream:get_body_as_string()
    if body ~= nil then
      print("LUA: Remote URI success..")
      return "rtsp://" .. camera_username .. ":" .. camera_password .. "@" .. camera_remote_url .. ":" .. camera_remote_media_port .. "/videoMain"
    else
      print("LUA: Remote URI failed..")
    end
  else
    print("LUA: Remote URI failed..")
  end
  return nil
end
