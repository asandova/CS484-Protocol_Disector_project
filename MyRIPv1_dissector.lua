MyRIPv1_protocol = Proto("MyRIPv1", "This is a simple distance vector protocol. It has been enhanced with various techniques, including Split Horizon and Poison Reverse in order to enable it to perform better in somewhat complicated networks.")

Command_field = ProtoField.int32("MyRIPv1.Command", "Command",base.DEC)
Version_field = ProtoField.int32("MyRIPv1.Version", "Version",base.DEC)
Address_Family_field = ProtoField.string("MyRIPv1.Address_Family", "Address_Family",base.ASCII)
IPv4_Address_field = ProtoField.string("MyRIPv1.IPv4_Address", "IPv4_Address",base.ASCII)
Metric_field = ProtoField.int32("MyRIPv1.Metric", "Metric",base.DEC)

MyRIPv1_protocol.fields = {
	Command_field,
	Version_field,
	Address_Family_field,
	IPv4_Address_field,
	Metric_field
}

function MyRIPv1_protocol.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = MyRIPv1_protocol.name

	local subtree = tree:add(MyRIPv1_protocol, buffer(),"MyRIPv1 Protocol data")

	local Command_value = buffer( 0, 1 ):uint()

	local Command_description = get_Command( Command_value )
	subtree:add(Command_field, buffer(0, 1)):append_text(" (" .. Command_description .. ")")

	subtree:add(Version_field, buffer(1, 1))
	subtree:add(Address_Family_field, buffer(4, 2))
	subtree:add(IPv4_Address_field, buffer(8, 4))
	subtree:add(Metric_field, buffer(16, 4))
end

function get_Command( Command )
	local Command_name = "Unknown"

	if ( Command >= 1 and Command < 2 ) then Command_name = "Request"
	elseif ( Command >= 2 and Command < 3 ) then Command_name = "Responce"
	elseif ( Command >= 3 and Command < 4 ) then Command_name = "Trace on"
	elseif ( Command >= 4 and Command < 5 ) then Command_name = "Trance off"
	elseif ( Command >= 5 and Command < 6 ) then Command_name = "SUN Reserved"
	elseif ( Command >= 6 and Command < 7 ) then Command_name = "Triggered Request"
	elseif ( Command >= 7 and Command < 8 ) then Command_name = "Triggered Responce"
	elseif ( Command >= 8 and Command < 9 ) then Command_name = "Triggered Acknowlegement"
	elseif ( Command >= 9 and Command < 10 ) then Command_name = "Update Request"
	elseif ( Command >= 10 and Command < 11 ) then Command_name = "Update Responce"
	elseif ( Command >= 11 and Command < 12 ) then Command_name = "Update Ackowledge"
	else Command_name = "Unknown" end
	return Command_name
end
local udp_port = DissectorTable.get("udp.port")
udp_port:add(520, MyRIPv1_protocol)
