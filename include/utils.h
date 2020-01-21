String toString(IPAddress ip)
{
    String ipString = String(ip[0]);
    for (byte octet = 1; octet < 4; ++octet)
        ipString += '.' + String(ip[octet]);
    return ipString;
}