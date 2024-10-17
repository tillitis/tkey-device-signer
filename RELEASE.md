# Release notes


## v1.0.2

- Change license to BSD-2-Clause
- Follow REUSE specification, see https://reuse.software/
- Bump tkey-libs to v0.1.2, also using BSD-2-Clause
- Exit early in build scripts
- Added make target for creating compile_commands.json for clangd

**Note:** this release does not change the CDI generated for a signer
built with touch (the default).

Complete
[changelog](https://github.com/tillitis/tkey-device-signer/compare/v1.0.1...v1.0.2).

## v1.0.1

- Resolve a bug that prevents the signer, built without touch, to do a
  signature.
- Add clangd files to gitignore.
- Use the right shasum tool in Makefile.
- Format the app.bin.sha512 file correctly.

**Note:** this release does not change the CDI generated for a signer
built with touch (the default).

Complete
[changelog](https://github.com/tillitis/tkey-device-signer/compare/v1.0.0...v1.0.1).


## v1.0.0

- Fix TOCTOU bug.
- Remove prehash command.
- Introduce firmware hash command.
- Device app version now 3 since protocol changed.
- Introduce an explicit protocol state machine, exiting early on
  suspicious behaviour, [see implementation notes](docs/implementation-notes.md).
- Update to tkey-libs v0.1.1.
- Use CDI directly instead of copying.

Complete
[changelog](https://github.com/tillitis/tkey-device-signer/compare/v0.0.8...v1.0.0).

## v0.0.8

- Update to use tkey-libs v0.0.2, including Monocypher 4.0.1
- Use execution monitor
- Support signing pre-hashed messages.

NOTE: this release changes the Tkey identity (CDI) compared to v0.0.7,
i.e., you will not have the same Ed25519 keypair derived.

Complete
[changelog](https://github.com/tillitis/tkey-device-signer/compare/v0.0.7...v0.0.8).

## v0.0.7

Just ripped from

https://github.com/tillitis/tillitis-key1-apps

No semantic changes.
