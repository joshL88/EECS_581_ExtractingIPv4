/*
 * Program: IPv4 Address Extractor
 * Author: ChatGPT
 * Date: September 24, 2026
 *
 * Description:
 * This program reads lines of text and searches for a valid IPv4 address,
 * optionally followed by a port number. Garbage characters are ignored,
 * while candidate tokens containing digits, periods, and colons must match
 * the complete IPv4 address and optional port grammar.
 *
 * The program rejects invalid octets, leading zeros, invalid ports, and
 * partial matches. On success, the IPv4 address is displayed along with
 * its decimal value and port number, or "None" if no port is present.
 */
#include <iostream>
#include <string>

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort)
{
    // Default failure values
    outAddress = 0;
    outPort = -1;

    size_t i = 0;

    while (i < str.length())
    {
        // Skip garbage characters
        if (!(str[i] >= '0' && str[i] <= '9') &&
            str[i] != '.' && str[i] != ':')
        {
            i++;
            continue;
        }

        // Find the end of this candidate token.
        size_t start = i;
        while (i < str.length() &&
               ((str[i] >= '0' && str[i] <= '9') ||
                str[i] == '.' || str[i] == ':'))
        {
            i++;
        }

        size_t end = i;

        // Parse the entire candidate token.
        unsigned long octets[4] = {0, 0, 0, 0};
        int octetCount = 0;
        size_t pos = start;

        bool valid = true;

        // Parse exactly four octets
        for (int octet = 0; octet < 4; octet++)
        {
            // Must have at least one digit
            if (pos >= end || str[pos] < '0' || str[pos] > '9')
            {
                valid = false;
                break;
            }

            size_t digitStart = pos;
            unsigned long value = 0;

            // Read the digits
            while (pos < end && str[pos] >= '0' && str[pos] <= '9')
            {
                value = value * 10 + (str[pos] - '0');
                pos++;
            }

            size_t digitCount = pos - digitStart;

            // 1-3 digits only
            if (digitCount > 3)
            {
                valid = false;
                break;
            }

            // No leading zero unless the value is exactly 0
            if (digitCount > 1 && str[digitStart] == '0')
            {
                valid = false;
                break;
            }

            // Must be 0-255
            if (value > 255)
            {
                valid = false;
                break;
            }

            octets[octet] = value;
            octetCount++;

            // First three octets must have a period
            if (octet < 3)
            {
                if (pos >= end || str[pos] != '.')
                {
                    valid = false;
                    break;
                }

                pos++;
            }
        }

        // If the address itself was invalid, reject the entire token.
        if (!valid || octetCount != 4)
        {
            continue;
        }

        // Optional port
        int port = -1;

        if (pos < end)
        {
            // If anything remains, it must begin with ':'
            if (str[pos] != ':')
            {
                continue;
            }

            pos++;

            // Port must contain at least one digit
            if (pos >= end || str[pos] < '0' || str[pos] > '9')
            {
                continue;
            }

            size_t portStart = pos;
            unsigned long portValue = 0;

            while (pos < end && str[pos] >= '0' && str[pos] <= '9')
            {
                portValue = portValue * 10 + (str[pos] - '0');
                pos++;
            }

            size_t portDigits = pos - portStart;

            // 1-5 digits
            if (portDigits > 5)
            {
                continue;
            }

            // No leading zero unless value is exactly 0
            if (portDigits > 1 && str[portStart] == '0')
            {
                continue;
            }

            // 0-65535
            if (portValue > 65535)
            {
                continue;
            }

            port = static_cast<int>(portValue);
        }

        // Nothing in the candidate token should remain.
        if (pos != end)
        {
            continue;
        }

        // Construct the 32-bit IPv4 value.
        outAddress =
            (octets[0] << 24) |
            (octets[1] << 16) |
            (octets[2] << 8)  |
            octets[3];

        outPort = port;

        return true;
    }

    return false;
}


int main()
{
    std::string input;

    while (true)
    {
        std::cout << "Enter a string (or 'END' to quit): ";

        if (!std::getline(std::cin, input))
            break;

        if (input == "END")
        {
            std::cout << "Program terminated.\n";
            break;
        }

        unsigned long address;
        int port;

        if (extractIPv4(input, address, port))
        {
            // Convert the numeric address back to dotted decimal
            unsigned int a = (address >> 24) & 255;
            unsigned int b = (address >> 16) & 255;
            unsigned int c = (address >> 8) & 255;
            unsigned int d = address & 255;

            std::cout << "Extracted IPv4 address: "
                      << a << "." << b << "." << c << "." << d
                      << " (decimal value: " << address
                      << ", port: ";

            if (port == -1)
                std::cout << "none";
            else
                std::cout << port;

            std::cout << ")\n";
        }
        else
        {
            std::cout << "Invalid input: no valid IPv4 address found\n";
        }
    }

    return 0;
}