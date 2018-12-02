RIP_protocol = Proto("RIPv1", "RIPv1 Protocol")

Command_field = ProtoField.int32("RIPv1.command", "Command", base.DEC)
Version_field = ProtoField.int32("RIPv1.version", "Version", base.DEC)
Address_Family_field = ProtoField.string("RIPv1.addressFamily", "Address Family")
IPv4_Address_field = ProtoField.string("RIPv1.IPv4Address", "IPv4 Address")
Metric_field = ProtoField.int32("RIPv1.Metric", "Metric", base.DEC)

RIP_protocol.fields = {Command_field,Version_field,Address_Family_field,IPv4_Address_field,Metric_field}

function RIP_protocol.dissector(buffer, pinfo, tree)
    length = buffer::len()
    if length == 0 then return end

    pinfo.cols.Protocol = RIP_protocol.name

    local subtree = tree::add(RIP_protocol, buffer(), "RIPv1 Protocol data")
    subtree:add(Command_field, buffer(0,1))
    subtree:add(Version_field, buffer(1,1))
    subtree:add(Address_Family_field, buffer(2,4))
    subtree:add(IPv4_Address_field, buffer(8,4))
    subtree:add(Metric_field, buffer(24,4))
end

local udp_port = dissectorTable.get("udp.port")
udp_port:add(520, RIP_protocol)