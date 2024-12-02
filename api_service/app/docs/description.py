API_DESCRIPTION = """
# Sensor API Service

## Overview
This API service provides endpoints for managing sensor data with real-time updates via WebSocket.

## Features
* Real-time sensor data streaming
* Batch data processing
* Historical data analysis
* Authentication and authorization
* WebSocket support

## Authentication
The API uses JWT tokens for authentication. To access protected endpoints:
1. Obtain a token using the `/auth/token` endpoint
2. Include the token in the Authorization header: `Bearer <token>`

## WebSocket
To connect to the WebSocket endpoint:
1. Connect to `/ws/sensor/{sensor_id}`
2. Optionally include token in the connection query: `?token=<token>`

## Rate Limiting
* API calls are limited to 100 requests per minute per IP
* WebSocket connections are limited to 5 concurrent connections per user
"""

tags_metadata = [
    {
        "name": "sensors",
        "description": "Operations with sensor data",
    },
    {
        "name": "auth",
        "description": "Authentication operations",
    },
    {
        "name": "websocket",
        "description": "WebSocket operations for real-time data",
    },
] 