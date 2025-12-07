// signaling.js
import WebSocket, { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 8080 });
const clients = {};

wss.on('connection', ws => {
  ws.on('message', msg => {
    const data = JSON.parse(msg);
    if (data.type === 'register') {
      clients[data.id] = ws;
    } else if (data.type === 'signal') {
      const target = clients[data.target];
      if (target) target.send(JSON.stringify({ from: data.from, signal: data.signal }));
    }
  });

  ws.on('close', () => {
    for (const [id, c] of Object.entries(clients)) if (c === ws) delete clients[id];
  });
});

console.log("Signaling server running on ws://localhost:8080");

