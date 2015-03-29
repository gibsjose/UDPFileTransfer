import sys

def run():
    if(len(sys.argv) != 4):
        print("Error: calling syntax: python generateTestFile.py <output file name> <length of packet> <# of packets>")
        sys.exit(1)

    lOutputFile = sys.argv[1]
    lPacketLength = int(sys.argv[2])
    lNumPackets = int(sys.argv[3])

    print("Output file: {}".format(lOutputFile))
    print("Packet length: {}".format(lPacketLength))
    print("Number of packets: {}".format(lNumPackets))

    # Generate the output file.
    with open(lOutputFile, "w") as lFile:
        lMagicNumber = 0
        for x in range(0, lNumPackets):
            for y in range(0, lPacketLength):
                lFile.write(str(lMagicNumber))
            lMagicNumber = lMagicNumber + 1

if __name__ == '__main__':
    run()
