MyRTP_protocol = Proto("MyRTP", "RTP Protocol")

Version_field = ProtoField.int32("MyRTP.Version", "Version",base.DEC)
Padding_field = ProtoField.int32("MyRTP.Padding", "Padding",base.DEC)
Extension_field = ProtoField.int32("MyRTP.Extension", "Extension",base.DEC)
CSRC_Count_field = ProtoField.int32("MyRTP.CSRC_Count", "CSRC_Count",base.DEC)
Maker_field = ProtoField.int32("MyRTP.Maker", "Maker",base.DEC)
Payload_Type_field = ProtoField.int32("MyRTP.Payload_Type", "Payload_Type",base.DEC)
Sequence_Number_field = ProtoField.int32("MyRTP.Sequence_Number", "Sequence_Number",base.DEC)
Time_Stamp_field = ProtoField.int32("MyRTP.Time_Stamp", "Time_Stamp",base.DEC)
Synchronization_Source_field = ProtoField.int32("MyRTP.Synchronization_Source", "Synchronization_Source",base.DEC)
Contribution_Source_field = ProtoField.int32("MyRTP.Contribution_Source", "Contribution_Source",base.DEC)

MyRTP_protocol.fields = {
	Version_field,
	Padding_field,
	Extension_field,
	CSRC_Count_field,
	Maker_field,
	Payload_Type_field,
	Sequence_Number_field,
	Time_Stamp_field,
	Synchronization_Source_field,
	Contribution_Source_field
}

function MyRTP_protocol.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = MyRTP_protocol.name

	local subtree = tree:add(MyRTP_protocol, buffer(),"MyRTP Protocol data")
	subtree:add(Version_field, buffer(0, 1):bitfield( 0, 2))
	subtree:add(Padding_field, buffer(0, 1):bitfield( 2, 1))
	subtree:add(Extension_field, buffer(0, 1):bitfield( 3, 1))
	subtree:add(CSRC_Count_field, buffer(0, 4):bitfield( 4, 4))
	subtree:add(Maker_field, buffer(1, 1):bitfield( 0, 1))

	local Payload_Type_value = buffer(1, 1):bitfield( 1, 7 )
	local Payload_Type_description = get_Payload_Type( Payload_Type_value )
	subtree:add(Payload_Type_field, buffer(1, 1):bitfield( 1, 7)):append_text(" (" .. Payload_Type_description .. ")")

	subtree:add(Sequence_Number_field, buffer(2, 2))
	subtree:add(Time_Stamp_field, buffer(4, 4))
	subtree:add(Synchronization_Source_field, buffer(8, 4))
	subtree:add(Contribution_Source_field, buffer(16, 4))
end

function get_Payload_Type( Payload_Type )
	local Payload_Type_name = "Unknown"

	if ( Payload_Type >= 0 and Payload_Type < 1 ) then Payload_Type_name = "PCMU"
	elseif ( Payload_Type >= 1 and Payload_Type < 2 ) then Payload_Type_name = "1016"
	elseif ( Payload_Type >= 2 and Payload_Type < 3 ) then Payload_Type_name = "G721"
	elseif ( Payload_Type >= 3 and Payload_Type < 4 ) then Payload_Type_name = "GSM"
	elseif ( Payload_Type >= 4 and Payload_Type < 5 ) then Payload_Type_name = "G723"
	elseif ( Payload_Type >= 5 and Payload_Type < 6 ) then Payload_Type_name = "DV14"
	elseif ( Payload_Type >= 6 and Payload_Type < 7 ) then Payload_Type_name = "DV14"
	elseif ( Payload_Type >= 7 and Payload_Type < 8 ) then Payload_Type_name = "LPC"
	elseif ( Payload_Type >= 8 and Payload_Type < 9 ) then Payload_Type_name = "PCMA"
	elseif ( Payload_Type >= 9 and Payload_Type < 10 ) then Payload_Type_name = "G722"
	elseif ( Payload_Type >= 10 and Payload_Type < 11 ) then Payload_Type_name = "L16"
	elseif ( Payload_Type >= 11 and Payload_Type < 12 ) then Payload_Type_name = "L16"
	elseif ( Payload_Type >= 12 and Payload_Type < 13 ) then Payload_Type_name = "QCELP"
	elseif ( Payload_Type >= 13 and Payload_Type < 14 ) then Payload_Type_name = "CN"
	elseif ( Payload_Type >= 14 and Payload_Type < 15 ) then Payload_Type_name = "MPA"
	elseif ( Payload_Type >= 15 and Payload_Type < 16 ) then Payload_Type_name = "G728"
	elseif ( Payload_Type >= 16 and Payload_Type < 17 ) then Payload_Type_name = "DV14"
	elseif ( Payload_Type >= 17 and Payload_Type < 18 ) then Payload_Type_name = "DV14"
	elseif ( Payload_Type >= 18 and Payload_Type < 19 ) then Payload_Type_name = "G729"
	elseif ( Payload_Type >= 19 and Payload_Type < 20 ) then Payload_Type_name = "Reserved"
	elseif ( Payload_Type >= 25 and Payload_Type < 26 ) then Payload_Type_name = "CellB"
	elseif ( Payload_Type >= 26 and Payload_Type < 27 ) then Payload_Type_name = "JPEG"
	elseif ( Payload_Type >= 28 and Payload_Type < 29 ) then Payload_Type_name = "nv"
	elseif ( Payload_Type >= 31 and Payload_Type < 32 ) then Payload_Type_name = "H261"
	elseif ( Payload_Type >= 32 and Payload_Type < 33 ) then Payload_Type_name = "MPV"
	elseif ( Payload_Type >= 33 and Payload_Type < 34 ) then Payload_Type_name = "MP2T"
	elseif ( Payload_Type >= 34 and Payload_Type < 35 ) then Payload_Type_name = "H263"
	elseif ( Payload_Type >= 72 and Payload_Type < 77 ) then Payload_Type_name = "Reserved"
	elseif ( Payload_Type >= 96 and Payload_Type < 128 ) then Payload_Type_name = "Dynamic"
	else Payload_Type_name = "Unknown" end
	return Payload_Type_name
end
local udp_port = DissectorTable.get("udp.port")
udp_port:add(5004, MyRTP_protocol)
