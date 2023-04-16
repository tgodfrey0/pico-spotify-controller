# Pico Spotify Controller

A challenge for [COMP2215](https://www.southampton.ac.uk/courses/modules/comp2215)

## What is the challenge?

The aim of this task is to learn the [lwIP](https://www.nongnu.org/lwip/2_1_x/index.html) TLS stack, HTTP requests and some GPIO interrupts. This is designed for use on embedded devices and works on the Pico W.

The challenge is to implement a HTTPS client on the Pico W to send requests to the Spotify Web API to control playback. I connected this up to 4 buttons (on GPIO 10-13) which use interrupts to call the relevant functions. 

## TLS with TCP on lwIP?????

lwIP has a section on their wiki for `altcp`. This is application layer TCP which allows for TLS to be used. This enables you to send HTTPS requests to the server. lwIP TLS uses the mbedTLS library. 

## Prasing JSON responses

lwJSON is also included as a submodule to allow for easy parsing of the JSON responses from the server. This is necessary for getting the `access_token` and related data.

## Authentication flow

First, you need to make an application on the [Spotify developer dashboard](https://developer.spotify.com/dashboard). 

This project uses the [Authorisation Code Flow](https://developer.spotify.com/documentation/web-api/tutorials/code-flow) for authentication as it is designed for long-running applications.

First you need an authorisation token. To do so, you can send a GET request to the `/authorize` endpoint. I pasted the following URL into a browser and stored the `code` parameter that is returned. 

```https://accounts.spotify.com/authorize?client_id={CLIENT_ID}&response_type=code&redirect_uri={REDIRECT_URI}&scope=app-remote-control%20streaming%20user-read-playback-state%20user-modify-playback-state%20user-read-currently-playing&show_dialog
```

This redirects to the callback uri and has a `code` parameter in the uri which is the authorisation code.

Once you have an authorisation code, the applciation can request an access code. This should be done during runtime so that the application can renew the token using a refresh token that is sent with the access token. To get an access code a `POST` request should be sent to the `/api/token` endpoint with the `grant_type` parameter of `authorization_code`; the `code` parameter equal to the authorisation code and a redirect uri. The client ID and client secret must be included in a basic authorisation header separated with a colon an base 64 encoded.

Once you have an access token, normal requests can be sent.

## Resources 

- [Spotify Web API](https://developer.spotify.com/documentation/web-api/)
- [lwIP](https://www.nongnu.org/lwip/2_1_x/group__altcp__api.html)
- [lwJSON](https://docs.majerle.eu/projects/lwjson/en/latest/)
