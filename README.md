# dont_trust

Secure peer to peer communication, opsec focused

### Development environment
compile_flags.txt is for [usage with clangd](https://clangd.llvm.org/design/compile-commands#compilation-databases)<br>
The c standard being used is a dialect from c17, that being gnu17

## Building
### Dependencies
- gcc
- [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) (From openssl3) header files
- make (Or run the build command from the Makefile directly)

To build:
```bash
$ make build
```
## Usage
### Dependencies
- An [ANSI escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code) conformant terminal with E3 capability (Most of the terminals nowadays, you probably should not need to worry about it if you aren't using some exotic setup)
- Tor, [the central project, providing the core software for using and participating in the Tor network, not the browser](https://gitlab.torproject.org/tpo/core/tor/) (If you intend to establish connections over it)
- [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) from openssl3

##### You must provide your rsa private key and the rsa public key from your peer:
```bash
$ dont_trust --pKey "./client_private_key_path.txt" --pubKey "./peer_public_key_path.pem"
```
###### The file type does not matter but the keys must be [PEM encoded](https://www.rfc-editor.org/rfc/rfc7468)
###### The possible sizes are those supported by [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) from [openssl3](https://www.openssl.org/)

### Connecting
##### To connect to an ipv4 address:
```bash
$ dont_trust -t 192.168.0.1
```
###### The address must be human readable
##### To connect to an onion address:
```bash
$ dont_trust -o vww6ybal4bd7szmgncyruucpgfkqahzddi37ktceo3ah7ngmcopnpyyd.onion
```

### Listening for connections
##### To listen for normal ones:
```bash
$ dont_trust ...
```
###### Just dont use any connection option, that being "-t" and "-o"
##### To listen for onion ones:
Before that, do this first time setup:<br>
1 - Generate a hashed password for control port access, <your_password> length must not be bigger than 1000:
```bash
$ tor --hash-password <your_password>
```
2 - On your ["torrc" file](https://support.torproject.org/tbb/tbb-editing-torrc/), add the following:
```bash
ControlPort 9051
HashedControlPassword <The result from the above command>
```
3 - Reload the configs if tor is running:
```bash
$ sudo kill -s SIGHUP <tor process id>
```
<br>

Just provide your password
```bash
$ dont_trust --torControlPassword <your_password>
```

## Protocol Specification

Runs on top of tcp, there are 2 commands available:
###### A command is considered a network byte order 8 bits unsigned integer sent over the tcp byte stream

### 0x01 = Starts a message
After which should be on the following order:<br>

- An unsigned 32 bit number in network byte order, that's the message code
- An unsigned 32 bit number in network byte order containing the cipher text size in bytes
- The initialization vector used to decrypt the cipher text, a sequence of 16 bytes
- The private key encrypted session key (AES in this case) used to decrypt the ciphertext, a sequence of 256 bytes
- The cipher text, up to a maximum of 65552 bytes length

### 0x02 = Starts a message confirmation
After which should be on the following order:<br>

- An unsigned 32 bit number in network byte order, that's the number of confirmations codes
- n unsigned 32 bit numbers in network byte order, each being a message code from a particular message (Only your own messages have their code saved in the application, that's implementation dependent)
