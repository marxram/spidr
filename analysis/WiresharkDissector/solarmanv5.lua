solarmanv5_protocol = Proto("SolarmanV5",  "SolarmanV5 Protocol")

message_start = ProtoField.uint8("solarmanv5.message_start", "Start", base.HEX)
message_length = ProtoField.uint16("solarmanv5.message_length", "Length", base.DEC)
message_type = ProtoField.uint16("solarmanv5.message_type", "Message Type", base.HEX)
message_serial_unit = ProtoField.uint8("solarmanv5.message_serial_unit", "Serial (Unit)", base.HEX)
message_serial_cloud = ProtoField.uint8("solarmanv5.message_serial_cloud", "Serial (Cloud)", base.HEX)
logger_serial = ProtoField.uint32("solarmanv5.logger_serial", "Logger Serial", base.DEC)
frame_type = ProtoField.uint8("solarmanv5.frame_type", "Frame Type", base.HEX)
status = ProtoField.uint8("solarmanv5.status", "Status", base.HEX)
sensor_type = ProtoField.uint16("solarmanv5.sensor_type", "Sensor Type", base.HEX)
delivery_time = ProtoField.uint32("solarmanv5.delivery_time", "Delivery Time", base.DEC)
poweron_time = ProtoField.uint32("solarmanv5.poweron_time", "Power On Time", base.DEC)
offset_time = ProtoField.uint32("solarmanv5.offset_time", "Offset Time", base.DEC)
upload_period = ProtoField.uint8("solarmanv5.upload_period", "Upload Period", base.DEC)
data_acquisition_period = ProtoField.uint8("solarmanv5.data_acquisition_period", "Data Acquisition Period", base.DEC)
heart_rate = ProtoField.uint32("solarmanv5.heart_rate", "Heart Rate", base.DEC)
modbus_payload = ProtoField.bytes("solarmanv5.modbus_payload", "Modbus Payload")
signal_strength = ProtoField.uint8("solarmanv5.signal_strength", "Signal Strength", base.DEC)
module_version = ProtoField.string("solarmanv5.module_version", "Module Version", base.ASCII)
mac_address = ProtoField.bytes("solarmanv5.mac_address", "MAC Address")
local_ip = ProtoField.string("solarmanv5.local_ip", "Local IP Number", base.ASCII)
at_update_command = ProtoField.uint8("solarmanv5.at_update_command", "AT+ Update Command Supported (?)", base.HEX)
extd_system_version = ProtoField.string("solarmanv5.extd_system_version", "Extended System Version", base.ASCII)
protocol_upgrade_mthd = ProtoField.uint8("solarmanv5.protocol_upgrade_mthd", "Protocol Upgrade Method (?)", base.HEX)
-- 4210
inverter_serial = ProtoField.string("solarmanv5.inverter_serial", "Inverter Serial", base.ASCII)
daily_energy = ProtoField.uint32("solarmanv5.daily_energy", "Daily Energy", base.DEC)
total_energy = ProtoField.uint32("solarmanv5.total_energy", "Total Energy", base.DEC)
grid_voltage = ProtoField.uint16("solarmanv5.grid_voltage", "Grid Voltage", base.DEC)
grid_frequency = ProtoField.uint16("solarmanv5.grid_frequency", "Grid Frequency", base.DEC)
power = ProtoField.uint16("solarmanv5.power", "Power", base.DEC)
heatsink_temperature = ProtoField.int16("solarmanv5.heatsink_temperature", "Heatsink Temperature", base.DEC)
vdc1 = ProtoField.uint16("solarmanv5.vdc1", "VDC1", base.DEC)
idc1 = ProtoField.uint16("solarmanv5.idc1", "IDC1", base.DEC)
vdc2 = ProtoField.uint16("solarmanv5.vdc2", "VDC2", base.DEC)
idc2 = ProtoField.uint16("solarmanv5.idc2", "IDC2", base.DEC)
inverter_comm_prot_ver = ProtoField.string("solarmanv5.inverter_comm_prot_ver", "Inverter Communication Protocol Version", base.ASCII)
inverter_ctrl_fw_ver = ProtoField.string("solarmanv5.inverter_ctrl_fw_ver", "Inverter Control Board Firmware Version", base.ASCII)
inverter_comm_fw_ver = ProtoField.string("solarmanv5.inverter_comm_fw_ver", "Inverter Communication Board Firmware Version", base.ASCII)
inverter_rated_power = ProtoField.uint16("solarmanv5.inverter_rated_power", "Inverter Rated Power", base.DEC)
module1_serial = ProtoField.string("solarmanv5.module1_serial", "Module 1 Serial", base.ASCII)
module2_serial = ProtoField.string("solarmanv5.module2_serial", "Module 2 Serial", base.ASCII)
module3_serial = ProtoField.string("solarmanv5.module3_serial", "Module 3 Serial", base.ASCII)
module4_serial = ProtoField.string("solarmanv5.module4_serial", "Module 4 Serial", base.ASCII)
overfrequency = ProtoField.uint16("solarmanv5.overfrequency", "Overfrequency and Load Reduction Starting Point", base.DEC)
overfreq_percentage = ProtoField.uint16("solarmanv5.overfreq_percentage", "Overfrequency and Load Reduction Percentage", base.DEC)
grid_volt_ul = ProtoField.uint16("solarmanv5.grid_volt_ul", "Grid Voltage Upper Limit", base.DEC)
grid_volt_ll = ProtoField.uint16("solarmanv5.grid_volt_ll", "Grid Voltage Lower Limit", base.DEC)
grid_freq_ul = ProtoField.uint16("solarmanv5.grid_freq_ul", "Grid Frequency Upper Limit", base.DEC)
grid_freq_ll = ProtoField.uint16("solarmanv5.grid_freq_ul", "Grid Frequency Lower Limit", base.DEC)
selfcheck_time = ProtoField.uint16("solarmanv5.selfcheck_time", "Start-up Self-checking Time", base.DEC)
-- 4310 Networking Status?
wifi_ssid = ProtoField.string("solarmanv5.wifi_ssid", "WiFi SSID", base.ASCII)

-- end
message_checksum = ProtoField.uint8("solarmanv5.message_checksum", "Checksum", base.HEX)
message_end = ProtoField.uint8("solarmanv5.message_end", "End", base.HEX)

solarmanv5_protocol.fields = {message_start, message_length, message_type, message_serial_unit, message_serial_cloud, logger_serial, frame_type, status, sensor_type, delivery_time, poweron_time, offset_time, upload_period,   data_acquisition_period, heart_rate, modbus_payload, signal_strength, module_version, mac_address, local_ip, at_update_command, extd_system_version, protocol_upgrade_mthd, inverter_serial, daily_energy, total_energy, grid_voltage, grid_frequency, power, heatsink_temperature, vdc1, idc1, vdc2, idc2, inverter_comm_prot_ver, inverter_ctrl_fw_ver, inverter_rated_power, inverter_comm_fw_ver, module1_serial, module2_serial, module3_serial, module4_serial, overfrequency, overfreq_percentage, grid_volt_ul, grid_volt_ll, grid_freq_ul, grid_freq_ll, selfcheck_time, wifi_ssid,  message_checksum, message_end}

function solarmanv5_protocol.dissector(buffer, pinfo, tree)
  local length = buffer:len()
  if length == 0 then return end
  local msg_start = buffer(0,1):le_uint()
  if msg_start ~= 0xa5 then return end

  pinfo.cols.protocol = solarmanv5_protocol.name

  local subtree = tree:add(solarmanv5_protocol, buffer(), "SolarmanV5 Protocol")

  subtree:add_le(message_start,         buffer(0,1))

  local message_length_value = buffer(1,2):le_uint()
  subtree:add_le(message_length,  buffer(1,2))

  local control_code_value = buffer(3,2):le_uint()
  local control_code_name = get_control_code_name(control_code_value)
  subtree:add_le(message_type,          buffer(3,2)):append_text(" (" .. control_code_name .. ")")

  subtree:add_le(message_serial_cloud,  buffer(5,1))
  subtree:add_le(message_serial_unit,   buffer(6,1))
  subtree:add_le(logger_serial,         buffer(7,4))
 
  local frame_type_number = buffer(11,1):le_uint()
  local frame_type_name = get_frame_type_name(frame_type_number)
  subtree:add_le(frame_type,            buffer(11,1)):append_text(" (" .. frame_type_name .. ")")


  if control_code_value == 0x4110 then -- hello message, unit to cloud
    subtree:add_le(delivery_time,      buffer(12,4)):append_text(" s")
    subtree:add_le(poweron_time,       buffer(16,4)):append_text(" s")

    subtree:add (upload_period,        buffer(24,1)):append_text(" min")
    subtree:add (data_acquisition_period, buffer(25,1)):append_text(" s")
    subtree:add (heart_rate,           buffer(26,1)):append_text(" s")

    subtree:add (signal_strength,      buffer( 28,  1))
    subtree:add (module_version,       buffer( 30, 40))

    subtree:add (mac_address,          buffer( 70,  6)):append_text(" (" .. tostring(buffer(70,6):ether()) .. ")")  -- :append_text(" (" .. buffer( 70,  6):bytes():tohex(false, ":") .. ")")
    subtree:add (local_ip,             buffer( 76, 16))

    subtree:add (at_update_command,    buffer( 99,  1))
    subtree:add (extd_system_version,  buffer(100, 40))
    subtree:add (protocol_upgrade_mthd,buffer(140,  1))

    subtree:add (wifi_ssid,             buffer(183, 32))  

    subtree:add_le(message_checksum,   buffer(message_length_value+11,1)) :append_text(" [unverified]")
    subtree:add_le(message_end,        buffer(message_length_value+12,1)) :append_text(" [always 0x15]")


  elseif control_code_value == 0x4210 then -- data message, unit to cloud
    subtree:add_le(sensor_type,        buffer(12,2))

    local offset_time_value = buffer(22,4):le_uint()
    local offset_time_text = get_datetime_text(offset_time_value) 
    local delivery_time_value = buffer(14,4):le_uint()
    local delivery_time_text = get_datetime_text(offset_time_value+delivery_time_value)
    local poweron_time_value = buffer(18,4):le_uint()
    local poweron_time_text = get_time_text(poweron_time_value)
  
    subtree:add_le (delivery_time,     buffer(14,4)):append_text(" (" .. delivery_time_text .. ")")
    subtree:add_le (poweron_time,      buffer(18,4)):append_text(" (" .. poweron_time_text .. ")")
    subtree:add_le (offset_time,       buffer(22,4)):append_text(" (" .. offset_time_text .. ")")

    subtree:add_le(status,             buffer(26,1)) :append_text(" [0x01 = OK]")

    subtree:add (inverter_serial,      buffer( 32, 10))  

    subtree:add_le (daily_energy,      buffer( 44,  4)):append_text(" (" .. string.format("%.2f",tostring((buffer(44,4):le_uint())/100)) .. " kWh)")
    subtree:add_le (total_energy,      buffer( 48,  4)):append_text(" (" .. string.format("%.1f",tostring((buffer(48,4):le_uint())/10)) .. " kWh)")
   
    subtree:add_le (grid_voltage,      buffer( 56,  2)):append_text(" (" .. string.format("%.1f",tostring((buffer(56,2):le_uint())/10)) .. " V)")
    subtree:add_le (grid_frequency,    buffer( 68,  2)):append_text(" (" .. string.format("%.2f",tostring((buffer(68,2):le_uint())/100)) .. " Hz)")
    subtree:add_le (power,             buffer( 70,  2)):append_text(" (" .. tostring(buffer(70,2):le_uint()) .. " W)")

    subtree:add_le (heatsink_temperature, buffer( 74,  2)):append_text(" (" .. string.format("%.2f",tostring((buffer( 74,  2):le_int())/100)) .. "°C)")

    subtree:add_le (vdc1,              buffer( 96,  2)):append_text(" (" .. string.format("%.1f",tostring((buffer(96,2):le_uint())/10)) .. " V)")
    subtree:add_le (idc1,              buffer( 98,  2)):append_text(" (" .. string.format("%.1f",tostring((buffer(98,2):le_uint())/10)) .. " A)")
    subtree:add_le (vdc2,              buffer(100,  2)):append_text(" (" .. string.format("%.1f",tostring((buffer(100,2):le_uint())/10)) .. " V)")
    subtree:add_le (idc2,              buffer(102,  2)):append_text(" (" .. string.format("%.1f",tostring((buffer(102,2):le_uint())/10)) .. " A)")

    subtree:add (inverter_comm_prot_ver, buffer(112, 8))
    subtree:add (inverter_ctrl_fw_ver, buffer(120, 8))
    subtree:add (inverter_comm_fw_ver, buffer(128, 8))

    -- Read, scale, and format the inverter rated power
    local rated_power_value = buffer(140, 2):le_uint()
    local scaled_power_value = rated_power_value / 10.0

    local formatted_power = string.format("%.1f W", scaled_power_value)
    subtree:add (inverter_rated_power, buffer(140, 2)):append_text(" (" .. formatted_power .. ")")

    subtree:add (module1_serial,       buffer(170, 16))  
    subtree:add (module2_serial,       buffer(186, 16))  
    subtree:add (module3_serial,       buffer(202, 16))  
    subtree:add (module4_serial,       buffer(218, 16))  

    subtree:add (overfrequency,        buffer(234,  2)):append_text(" (" .. string.format("%.2f",tostring((buffer(234,2):uint())/100)) .. " Hz)")
    subtree:add (overfreq_percentage,  buffer(236,  2)):append_text(" (" .. buffer(236,2):uint() .. "%)")

    subtree:add (grid_volt_ul,         buffer(246,  2)):append_text(" (" .. string.format("%.1f",buffer(246,2):uint()/10) .. " V)")
    subtree:add (grid_volt_ll,         buffer(248,  2)):append_text(" (" .. string.format("%.1f",buffer(248,2):uint()/10) .. " V)")
    subtree:add (grid_freq_ul,         buffer(250,  2)):append_text(" (" .. string.format("%.2f",buffer(250,2):uint()/100) .. " Hz)")
    subtree:add (grid_freq_ll,         buffer(252,  2)):append_text(" (" .. string.format("%.2f",buffer(252,2):uint()/100) .. " Hz)")

    subtree:add (selfcheck_time,       buffer(254,  2)):append_text(" (" .. buffer(254,2):uint() .. " s)")

    subtree:add_le(message_checksum,   buffer(message_length_value+11,1)) :append_text(" [unverified]")
    subtree:add_le(message_end,        buffer(message_length_value+12,1)) :append_text(" [always 0x15]")
    
  elseif control_code_value == 0x4810 then -- hello_end message
    local offset_time_value = buffer(20,4):le_uint()
    local offset_time_text = get_datetime_text(offset_time_value)
    local delivery_time_value = buffer(12,4):le_uint()
    local delivery_time_text = get_datetime_text(offset_time_value+delivery_time_value)
    local poweron_time_value = buffer(16,4):le_uint()
    local poweron_time_text = get_time_text(poweron_time_value)
  
    subtree:add_le(delivery_time,      buffer(12,4)):append_text(" (" .. delivery_time_text .. ")")
    subtree:add_le(poweron_time,       buffer(16,4)):append_text(" (" .. poweron_time_text .. ")")
    subtree:add_le(offset_time,        buffer(20,4)):append_text(" (" .. offset_time_text .. ")")

    subtree:add_le(status,             buffer(24,1)) :append_text(" [0x01 = OK]")

    -- 0x05
    -- 0x2c
    -- 44 * 0xFF
    
    subtree:add_le(message_checksum,   buffer(message_length_value+11,1)) :append_text(" [unverified]")
    subtree:add_le(message_end,        buffer(message_length_value+12,1)) :append_text(" [always 0x15]")


  elseif control_code_value == 0x4310 then -- Network Status
    subtree:add (wifi_ssid,            buffer(26, 24))

    subtree:add_le(message_checksum,   buffer(message_length_value+11,1)) :append_text(" [unverified]")
    subtree:add_le(message_end,        buffer(message_length_value+12,1)) :append_text(" [always 0x15]")
      


  elseif control_code_value == 0x4710 then -- Keep alive message, unit to cloud [complete]
    subtree:add_le(message_checksum,   buffer(12,1)) :append_text(" [unverified]")
    subtree:add_le(message_end,        buffer(13,1)) :append_text(" [always 0x15]")
  elseif control_code_value == 0x1110 -- Hello response, cloud to unit
      or control_code_value == 0x1210 -- data response, cloud to unit
      or control_code_value == 0x1710 -- keep alive response, cloud to unit
      or control_code_value == 0x1810 -- hello_end response, cloud to unit
    then 
    subtree:add_le(status,             buffer(12,1))   :append_text(" [0x01 = OK]")

    local delivery_time_value = buffer(13,4):le_uint()
    local delivery_time_text = get_datetime_text(delivery_time_value)
  
    subtree:add_le(delivery_time,      buffer(13,4)):append_text(" (" .. delivery_time_text .. ")")
    subtree:add_le(heart_rate,         buffer(17,4)):append_text(" seconds")

    subtree:add_le(message_checksum,   buffer(21,1)) :append_text(" [unverified]")
    subtree:add_le(message_end,        buffer(22,1)) :append_text(" [always 0x15]")
  end
    
end

function get_control_code_name(control_code_value)
  local control_code_name = "Unknown"

      if control_code_value == 0x4110 then control_code_name = "Hello message, unit to cloud"
  elseif control_code_value == 0x1110 then control_code_name = "Hello response, cloud to unit"
  elseif control_code_value == 0x4210 then control_code_name = "Data message, unit to cloud"
  elseif control_code_value == 0x1210 then control_code_name = "Data response, cloud to unit" 
  elseif control_code_value == 0x4710 then control_code_name = "Keep alive message, unit to cloud"
  elseif control_code_value == 0x1710 then control_code_name = "Keep alive response, cloud to unit" 
  elseif control_code_value == 0x4810 then control_code_name = "Hello_end message, unit to cloud"
  elseif control_code_value == 0x1810 then control_code_name = "Hello_end response, cloud to unit"
  elseif control_code_value == 0x4310 then control_code_name = "Network Status"
  end
-- Unknown: 4810, 1810. 

  return control_code_name
end


function get_frame_type_name(frame_type)
  local frame_type_name = "Unknown"

      if frame_type == 0 then frame_type_name = "keep alive"
  elseif frame_type == 1 then frame_type_name = "Data Logging Stick"
  elseif frame_type == 2 then frame_type_name = "Solar Inverter" end

  return frame_type_name
end

function get_datetime_text(time_value)
  local datetime_text = "Unknown"
  datetime_text = os.date("!%c", tonumber(time_value))
  return datetime_text
end

function get_time_text(time_value)
  local time_text = "Unknown"
  time_text = os.date("!%X", tonumber(time_value))
  return time_text
end


local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(10000, solarmanv5_protocol)