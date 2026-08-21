# Smart Energy Meter Web

Firmware ESP32 untuk membaca PZEM-004T, menampilkan data pada OLED dan dashboard web, serta mempublikasikan telemetri MQTT.

## Menyiapkan proyek

1. Salin `include/config.example.h` menjadi `include/config.local.h`.
2. Isi kredensial MQTT milik Anda sendiri. File lokal ini diabaikan Git.
3. Isi sertifikat CA broker pada `MQTT_CA_CERT`. Jangan gunakan `MQTT_TLS_INSECURE` di perangkat produksi.
4. Build dan unggah firmware serta filesystem dengan PlatformIO:

   ```sh
   pio run -t upload
   pio run -t uploadfs
   ```

`data/` adalah aset dashboard yang dipasang ke LittleFS. Dashboard utama mengambil data dari WebSocket perangkat (`/ws`) dan API lokal, sehingga kredensial broker tidak perlu berada di browser.

## Menyiapkan Wi-Fi pelanggan

Setiap unit memakai firmware yang sama. Jika perangkat belum menyimpan Wi-Fi, OLED menampilkan nama jaringan sementara dan kunci, misalnya `SmartMeter-A1B2C3` dan `SEM-A1B2C3`.

1. Hubungkan ponsel ke jaringan tersebut menggunakan kunci di OLED.
2. Buka `http://192.168.4.1` jika halaman setup tidak terbuka otomatis.
3. Masukkan nama dan password Wi-Fi pelanggan.
4. Perangkat menyimpan konfigurasi di NVS, restart, lalu terhubung ke Wi-Fi tersebut.

Factory reset dari dashboard juga menghapus konfigurasi Wi-Fi dan mengembalikan perangkat ke mode setup.

## Catatan keamanan

Kredensial sebelumnya pernah tersimpan dalam riwayat repository. Rotasi segera kata sandi Wi-Fi dan MQTT tersebut, lalu gunakan kredensial baru hanya melalui `config.local.h`.

## Struktur

- `src/` — firmware ESP32.
- `include/` — header dan contoh konfigurasi.
- `data/` — dashboard statis untuk LittleFS.
