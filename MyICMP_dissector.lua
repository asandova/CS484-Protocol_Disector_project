MyICMP_protocol = Proto("MyICMP", "Description of protocol goes here")

Type_field = ProtoField.int32("MyICMP.Type", "Type",base.DEC)
Code_field = ProtoField.int32("MyICMP.Code", "Code",base.DEC)
ICMP_Checksum_field = ProtoField.int32("MyICMP.ICMP_Checksum", "ICMP_Checksum",base.DEC)
DATA_field = ProtoField.string("MyICMP.DATA", "DATA",base.ASCII)

MyICMP_protocol.fields = {
	Type_field,
	Code_field,
	ICMP_Checksum_field,
	DATA_field
}

function MyICMP_protocol.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = MyICMP_protocol.name

	local subtree = tree:add(MyICMP_protocol, buffer(),"MyICMP Protocol data")
	subtree:add(Type_field, buffer(0, 1))

	local Code_value = buffer( 1, 1 ):uint()

	local Code_description = get_Code( Code_value )
	subtree:add(Code_field, buffer(1, 1)):append_text(" (" .. Code_description .. ")")

	subtree:add_le(ICMP_Checksum_field, buffer(2, 2))
	subtree:add_le(DATA_field, buffer(4, length - 4))
end

function get_Code( Code )
	local Code_name = "Unknown"

	if ( Code >= 0 and Code < 1 ) then Code_name = "Echo Reply"
	elseif ( Code >= 3 and Code < 4 ) then Code_name = "Destination Unreachable"
	elseif ( Code >= 4 and Code < 5 ) then Code_name = "Source Quench"
	elseif ( Code >= 5 and Code < 6 ) then Code_name = "Redirect"
	elseif ( Code >= 6 and Code < 7 ) then Code_name = "Alternate host address"
	elseif ( Code >= 8 and Code < 9 ) then Code_name = "Echo Request"
	elseif ( Code >= 9 and Code < 10 ) then Code_name = "Router Advertisement"
	elseif ( Code >= 10 and Code < 11 ) then Code_name = "Router Solicitation"
	elseif ( Code >= 11 and Code < 2 ) then Code_name = "Time Exceeded"
	elseif ( Code >= 12 and Code < 13 ) then Code_name = "Parameter Problem"
	elseif ( Code >= 13 and Code < 14 ) then Code_name = "Timestamp Request"
	elseif ( Code >= 14 and Code < 15 ) then Code_name = "Timestamp reply"
	elseif ( Code >= 15 and Code < 16 ) then Code_name = "Infomation Request"
	elseif ( Code >= 16 and Code < 17 ) then Code_name = "Infomation Reply"
	elseif ( Code >= 17 and Code < 18 ) then Code_name = "Address Mask Request"
	elseif ( Code >= 18 and Code < 19 ) then Code_name = "Address Mask reply"
	elseif ( Code >= 19 and Code < 20 ) then Code_name = "Reserved"
	elseif ( Code >= 20 and Code < 30 ) then Code_name = "Reserved"
	elseif ( Code >= 30 and Code < 31 ) then Code_name = "Traceroute"
	elseif ( Code >= 31 and Code < 32 ) then Code_name = "Conversion Error"
	elseif ( Code >= 32 and Code < 33 ) then Code_name = "Mobile Host Redirect"
	elseif ( Code >= 33 and Code < 34 ) then Code_name = "IPv6 Where-Are-You"
	elseif ( Code >= 34 and Code < 35 ) then Code_name = "IPv6 I-Am-Here"
	elseif ( Code >= 35 and Code < 36 ) then Code_name = "Mobile Registration Request"
	elseif ( Code >= 36 and Code < 37 ) then Code_name = "Mobile Registration Reply"
	elseif ( Code >= 37 and Code < 38 ) then Code_name = "Domain Name Request"
	elseif ( Code >= 38 and Code < 39 ) then Code_name = "Domain Name reply"
	elseif ( Code >= 39 and Code < 40 ) then Code_name = "SKIP Algorithm Discovery Protocol"
	elseif ( Code >= 40 and Code < 41 ) then Code_name = "Photuris, Security Failures"
	elseif ( Code >= 41 and Code < 42 ) then Code_name = "Experomental Mobility Protocol"
	elseif ( Code >= 42 and Code < 256 ) then Code_name = "Reserved"
	else Code_name = "Unknown" end
	return Code_name
end
local ip_proto = DissectorTable.get("ip.proto")
ip_proto:add(1, MyICMP_protocol)
