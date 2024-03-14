#### 🚧 Work in progress 🚧
# dont_trust

Secure peer to peer communication, opsec focused

### Motivation

- The need for information confidentiality
- Guarantee that the software does only what is intended
- No time to manually check the code of big open source projects
- No need for large communication protocols

### Philosophy/Goals

- Small codebase, easy to understand and hack
- Secure by default
- Useful as a teaching/learning platform for linux system programming
- Small number of source files
- Minimal dependencies

### Development environment
compile_flags.txt is for [usage with clangd](https://clangd.llvm.org/design/compile-commands#compilation-databases)
The c standard being used is a dialect from c17, that being gnu17

## Building
### Dependencies
- gcc
- openssl3 development files
- [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html)

To build:
```bash
$ make build
```
To run:
```bash
$ ./build/dont_trust ...
```

## Usage

##### You must provide your rsa private key and the rsa public key from your peer (⚠️Not yet implemented):
```bash
$ dont_trust -pri "./client_public_key_path.txt" -pub "./peer_private_key_path.pem"
```
###### The file type does not matter but the keys must be [PEM encoded](https://www.rfc-editor.org/rfc/rfc7468)
###### The possible sizes are those supported by [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) from [openssl3](https://www.openssl.org/)
##### To connect to an ipv4 address:
```bash
$ dont_trust -t 192.168.0.1
```
###### The address must be human readable
##### To connect to an onion address:
```bash
$ dont_trust -o vww6ybal4bd7szmgncyruucpgfkqahzddi37ktceo3ah7ngmcopnpyyd.onion
```

## Protocol Specification, for now...

Runs on top of tcp, there are 2 commands available:
###### A command is considered a network byte order 8 bits unsigned integer sent over the tcp byte stream

### 0x01 = Starts a message
After which should be on the following order:<br>

- An unsigned 32 bit number in network byte order, that's the message code
- An unsigned 16 bit number in network byte order containing the message size in bytes (Up to a maximum of 65536)
- The acutal message

### 0x02 = Starts a message confirmation
After which should be on the following order:<br>

- An unsigned 32 bit number in network byte order, that's the number of confirmations codes
- n unsigned 32 bit numbers in network byte order, each being a message code from a particular message (Only your own messages have their code saved in the application, that's implementation dependent)
