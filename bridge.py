#!/usr/bin/env python3
# bridge.py – leser JSON fra VM8 (stdin) og sender via WebSocket

import asyncio
import websockets
import sys

clients = set()

async def handler(websocket):
    clients.add(websocket)
    try:
        await websocket.wait_closed()
    finally:
        clients.remove(websocket)

async def read_stdin():
    loop = asyncio.get_event_loop()
    reader = asyncio.StreamReader()
    await loop.connect_read_pipe(lambda: asyncio.StreamReaderProtocol(reader), sys.stdin)
    while True:
        line = await reader.readline()
        if not line:
            break
        msg = line.decode().strip()
        if clients:
            await asyncio.gather(*[c.send(msg) for c in clients])

async def main():
    print("WebSocket-server på ws://localhost:8765", flush=True)
    async with websockets.serve(handler, "localhost", 8765):
        await read_stdin()

asyncio.run(main())
