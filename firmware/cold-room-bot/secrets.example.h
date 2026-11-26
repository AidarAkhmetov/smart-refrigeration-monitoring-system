// Copy this file to secrets.h and fill in your own values.
// secrets.h is listed in .gitignore and must never be committed.
#pragma once

#define WIFI_SSID       "your-wifi-name"
#define WIFI_PASSWORD   "your-wifi-password"

// Token from @BotFather, looks like 1234567890:AA...
#define BOT_TOKEN       "PASTE_TOKEN_HERE"

// Only this Telegram chat may use the bot and gets the alerts.
// Leave 0 at first: the bot then answers any chat with its chat id only,
// so you can find yours by writing to the bot.
#define ALLOWED_CHAT_ID 0LL

// Optional: the laptop receiver (status page). Leave "" to disable.
#define RECEIVER_URL    ""
