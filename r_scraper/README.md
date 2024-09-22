# Scraper

## Setup

#### Package installation

No need to install packages separately since cargo will do it for you

    Environment variables

```javascript
DB_URL="postgresql://user:password@localhost:5432/database_name?schema=public&connection_limit=10"
WEB_DRIVER_URL=http://localhost:4444
```

In order to run this project you need a **Postgresql database** and some kind of **web driver (protocol of talking to a browser) implementation**

I'm using [seleniarm/standalone-chromium](https://hub.docker.com/r/seleniarm/standalone-chromium) docker container.
In order to run this docker container I use the following command

```sh
docker run -d -p 4444:4444 -p 7900:7900 --shm-size="2g"  -e SE_NODE_MAX_SESSIONS=5 -e SE_VNC_NO_PASSWORD=1 seleniarm/standalone-chromium:latest
```

`4444` port is for web driver
`7900` is for VNC

`SE_NODE_MAX_SESSIONS` env param restricts a number of windows you can open at once

`SE_VNC_NO_PASSWORD` allows you to access VNC at url `http://localhost:7900` without password (the default btw is `secret`)

## Building

To do a debug run

```sh
cargo r
```

For a production run

```sh
cargo r --release
```
