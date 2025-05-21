#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <cstdint>
#include <cmath>

// Dictionary to store precomputed compressed values
std::unordered_map<float, uint16_t> compressionMap;
std::unordered_map<uint16_t, float> decompressionMap;

// Function to initialize the dictionary (precompute values)
void initializeCompressionMap() {
    for (int i = -9999999; i <= 9999999; ++i) {
        float value = i / 10000.0f; // Ensuring 4-digit precision
        uint16_t compressedValue = static_cast<uint16_t>(i & 0xFFFF); // Store in 16-bit
        compressionMap[value] = compressedValue;
        decompressionMap[compressedValue] = value;
    }
}

// Structure for the data packet (Strictly 8 bytes)
struct DataPacket {
    uint8_t address;  // 1 byte for filtering
    uint64_t compressedData; // 7 bytes for packed x, y, z values
};

// Function to compress float using the dictionary
uint16_t compressFloat(float value) {
    return compressionMap[value];
}

// Function to decompress float using the dictionary
float decompressFloat(uint16_t compressed) {
    return decompressionMap[compressed];
}

// Broadcaster function to send compressed data
DataPacket broadcast(float x, float y, float z, uint8_t address) {
    uint64_t compressedX = compressFloat(x);
    uint64_t compressedY = compressFloat(y);
    uint64_t compressedZ = compressFloat(z);
    uint64_t compressedData = (compressedX << 32) | (compressedY << 16) | compressedZ;
    return {address, compressedData};
}

// Listener function to receive and process data
void listener(const DataPacket& packet, uint8_t listener_address) {
    if (packet.address == listener_address) {
        uint64_t compressedData = packet.compressedData;
        float x = decompressFloat((compressedData >> 32) & 0xFFFF);
        float y = decompressFloat((compressedData >> 16) & 0xFFFF);
        float z = decompressFloat(compressedData & 0xFFFF);

        std::cout << "Listener " << (int)listener_address << " accepted data: "
                  << std::fixed << std::setprecision(4)
                  << x << ", " << y << ", " << z << std::endl;
    } else {
        std::cout << "Listener " << (int)listener_address << " rejected data.\n";
    }
}

int main() {
    initializeCompressionMap(); // Precompute dictionary

    // Example transmission
    float x = 1234.5678, y = -2345.6789, z = 3456.7890; // Ensure 4 digits before and after decimal
    uint8_t broadcaster_address = 10;

    DataPacket packet = broadcast(x, y, z, broadcaster_address);

    // Simulate three listeners with different addresses
    listener(packet, 10); // Should accept
    listener(packet, 25); // Should reject
    listener(packet, 10); // Should accept

    return 0;
}

