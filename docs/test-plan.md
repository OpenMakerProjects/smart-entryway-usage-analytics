# Test plan

Host tests cover interval attribution, exact hour/day boundaries,24/7slot eviction, toggles and millis wrap. CI actually compiles NodeMCU firmware and checks image/artifacts. Hardware: confirmI2C0x76/0x3C, enable for60s viaUSBON thenOFF, compareday_ms≈60000; disconnectsensor andconfirmLEDoff/validfalse. Restore, checkcountsresumeonlyifrequested. HA/Matter pairing tests unperformed.
