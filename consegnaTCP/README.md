# I Assegnazione di Laboratorio: Socket TCP — Gruppo G34

- **Corso:** Reti di Calcolatori (ITPS L-Z) — A.A. 2026/2027 (Prof.ssa Nicole Novielli)
- **Gruppo:** `G34`
- **Componenti:**
  - Donato Umberto Muscillo (Matricola: 852562)
  - Fabio Nigro
- **Sistema Operativo in uso sulla macchina di sviluppo e test:**
  - **macOS (Apple Silicon / Darwin POSIX)** — Codice scritto in C standard completamente **portabile** sia su sistemi Unix/macOS (`sys/socket.h`, `arpa/inet.h`, `unistd.h`) sia su **Windows** (`winsock.h`, `WSAStartup`, `closesocket`, `WSACleanup` tramite direttive `#if defined WIN32`).

## Struttura della Consegna TCP
- `client-TCP_G34/` — Progetto Eclipse Client TCP (`src/client-TCP_G34.c`, `src/protocol.h`)
- `serverTCP_G34/` — Progetto Eclipse Server TCP (`src/serverTCP_G34.c`, `src/protocol.h`)

## Compilazione ed Esecuzione da Terminale (Senza eseguibili nel repository)
- **macOS / Linux:**
  ```bash
  gcc serverTCP_G34/src/serverTCP_G34.c -o /tmp/serverTCP_G34
  gcc client-TCP_G34/src/client-TCP_G34.c -o /tmp/client-TCP_G34
  ```
- **Windows (MinGW):**
  ```bash
  gcc serverTCP_G34/src/serverTCP_G34.c -o serverTCP_G34.exe -lwsock32
  gcc client-TCP_G34/src/client-TCP_G34.c -o client-TCP_G34.exe -lwsock32
  ```
