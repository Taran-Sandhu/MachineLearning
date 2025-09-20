import asyncio
from typing import Dict, List

import httpx
import websockets
from fastapi import FastAPI, Query, WebSocket, WebSocketDisconnect, HTTPException
import numpy as np

try:
    import cppmodel
except Exception as e:
    cppmodel = None
    _CPPMODEL_IMPORT_ERROR = e

app = FastAPI()

BINANCE_URL = "https:


async def fetch_symbol(symbol: str, interval: str = "1m", limit: int = 500) -> List:
    params = {"symbol": symbol.upper(), "interval": interval, "limit": limit}
    async with httpx.AsyncClient() as client:
        resp = await client.get(BINANCE_URL, params=params)
        resp.raise_for_status()
        return resp.json()


def model_prediction(candles) -> float:
    data = [[float(c[i]) for i in range(6)] for c in candles]
    if cppmodel is None:
        raise HTTPException(
            status_code=500,
            detail=f"cppmodel extension not available: {_CPPMODEL_IMPORT_ERROR}",
        )
    return float(cppmodel.predict(data))


@app.get("/predict")
async def predict(symbol: str = Query(..., description="Trading pair symbol like BTCUSDT")):
    candles = await fetch_symbol(symbol)
    prediction = model_prediction(candles)
    return {"symbol": symbol.upper(), "prediction": prediction}


@app.websocket("/ws/predict")
async def ws_predict(websocket: WebSocket, symbol: str = Query(...)):
    await websocket.accept()
    try:
        while True:
            candles = await fetch_symbol(symbol)
            prediction = model_prediction(candles)
            await websocket.send_json({"symbol": symbol.upper(), "prediction": prediction})
            await asyncio.sleep(5)
    except WebSocketDisconnect:
        pass


UPSTREAM_EXECUTIONS_WS = "wss:


@app.websocket("/ws/executions")
async def ws_executions(websocket: WebSocket) -> None:
    await websocket.accept()
    try:
        async with websockets.connect(UPSTREAM_EXECUTIONS_WS) as upstream:
            async for message in upstream:
                await websocket.send_text(message)
    except WebSocketDisconnect:
        pass
    except Exception:
        await websocket.close(code=1011, reason="upstream error")


@app.get("/test")
async def test_model(
        symbol: str = Query(..., description="Trading pair symbol like BTCUSDT"),
        horizon: int = Query(5, description="Prediction horizon in bars"),
):
    candles = await fetch_symbol(symbol, interval="1m", limit=500)

    closes = [float(c[4]) for c in candles]

    y = []
    for i in range(horizon, len(closes)):
        ret = (closes[i] - closes[i - horizon]) / closes[i - horizon]
        y.append(1 if ret > 0 else 0)
    X = closes[:-horizon]

    split = int(len(X) * 0.7)
    y_train, y_test = y[:split], y[split:]
    X_train, X_test = X[:split], X[split:]

    preds = []
    for i in range(split, len(closes) - horizon):
        preds.append(1 if closes[i] > closes[i - 1] else 0)

    if not preds:
        return {"error": "Not enough data"}

    acc = float(np.mean(np.array(preds) == np.array(y_test)))

    return {
        "symbol": symbol.upper(),
        "samples": len(y_test),
        "accuracy": round(acc, 4),
    }


if __name__ == "__main__":
    import uvicorn
    import os

    port = int(os.environ.get("PORT", 5001))
    uvicorn.run(app, host="0.0.0.0", port=port)
