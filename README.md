#UDP File Transfer Notes

##Background
TCP File transfer was fairly easy:
* Automatically delivers packets in order
* Makes sure packets all got there

UDP is more complicated:
* Even error-checking is optional in UDP

##Project
###Requirements
1. Ensure no packets are lost
2. Ensure packets are delivered in order
3. Ensure there are no errors in the packets
4. Must send multiple packets at a time (**5 packets**)
5. Handle duplicate packets (lost `ACK`)

###Error Control Implementations
1. **CRC**
2. Checksum

###In-Order Delivery
1. Send sequence number, and sort them upon arrival

###Lost Packets
1. Can use sequence number
2. **Best way:** Wait for `ACK` after each packet sent, if no `ACK` within timeout period, resend it

###Multiple Packets
* *'Sliding Window'*
    * Can make all packets **equal length**
    * Sequence number example, `window_size = 5`
        * **[**1, 2, 3, 4, 5 **]**, 6, 7, 8, 9, 10
        * Any packets in window will be sent by the transmitter
        * The receiver will only wait for packets within window ('only packets w/ sequence of 4 greater or less')
* Window size of 5
* Make sequence number a 4-byte integer (can assume max file to send = `2^32` packets)

###Duplicate Packets
* `ACK` lost
* Sender will resend, ex, `4`, even though receiver already received it and sent `ACK`, but it never made it
    * Receiver can basically just ignore it, and send back an `ACK` again, since it already has it

###Example
1. Sender creates window of size 5, sending packets 1-5
2. Sender starts sending packet `1`
3. Receiver receives packet, send back an `ACK`, moves window forward by 1
4. Sender moves window forward by 1 (now includes packet 6), since it received `ACK`
5. Packet `2` lost (did not receive `ACK`), don't move window
6. Sender starts sending packet `3`
7. Receiver stores packet `3`, sends `ACK`
8. Sender can **NOT** move window forward, because `2` has not been sent
9. Sender resends `2` and receives `ACK`
10. Sender now moves window forward **2**, because it has now sent `2` and `3`

##Mininet
* Need a better way of testing, since using DataComm lab network will not result in packet loss, etc.
* `mininet` is a simulator, which runs in a virtual machine
* Files located on EOS in `/lab/networking/Mininet`
    * Copy all of these files to my directory on EOS
* Can also download virtual machine from **mininet**

###Running Virtual Machine
1. `vmplayer vmware/Mininet/Mininet.vmx` <--- After moving all files to `~/vmware/Mininet/`
2. **Login:**
    * Username: `mininet`
    * Password: `mininet`
3. Do **not** install the VMWare Tools Package
4. VM is Ubuntu Linux with networking tools added
5. Have full root access... can install whatever packages we want
6. Installing Java
    1. `sudo apt-get update`
    2. `sudo apt-get install default-jdk`
7. May want a graphical system
    1. `sudo apt-get update`
    2. Install **Gnome** if we want
8. SSH Workaround:
    1. Get IP Address `ifconfig -a`
    2. `ssh -Y mininet@192.168.117.128`
    3. Can start simulator via ssh
    4. **Start default config with windows we need:**
        1. `sudo mn -x`

###Starting Simulator
1. Start VM (**USE UBUNTU FOR THIS, NOT OS X**)
2. SSH Into VM from your machine
    * `ssh -Y mininet@<ip-address>`
3. Run Simulator
    * `sudo mn -x`
4. Four windows should pop up:
    1. **h1**: `Host 1`
    2. **h2**: `Host 2`
    3. **s1**: `Switch 1` between `Host 1` and `Host 2`
    4. **c0**: `Controller 0` don't actually need commands, can close this one
5. Hosts are 'separate' only for the purpose of the network
    * They share the same filesystem
    * Ex: When doing a file transfer, do it to a different directory or change the destination name
6. Can `ping` between hosts
7. Can run `wireshark` on one of the hosts
    * Run `wireshark` only on virtual hosts/switch, not on main VM `ssh` window

###Simulating
1. Simulating Packet Loss
    1. Go to switch terminal
    2. Run `tc qdisc add dev <INTERFACE> root netem loss 20%` <-- `tc` = 'Traffic Control'
        * **Interfaces**
            * `s1-eth1`: Interface with `Host 1`
            * `s1-eth2`: Interface with `Host 2`
            * To run on both interfaces: Execute command twice, with both identifiers
        * NOTE: Use `add` for the first time running. Use `change` for subsequent runs
        * May want to run on only **one** interface (only on one side of the switch) to simulate only packets being lost, or only `ACK`s being lost, but will test with both
2. Simulating Out-of-Order Packets
    1. Go to switch terminal
    2. Run `tc qdisc change dev <INTERFACE> netem delay 10ms reorder 20%`
        * Will send 20% of packets as normal
        * Remaining packets are delayed by 10ms, causing them to be out of order
