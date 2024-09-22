use reqwest::{
    header::{HeaderMap, HeaderValue, ACCEPT, USER_AGENT},
    Client,
};

use serde::{Deserialize, Serialize};
use serde_json::json;
use std::{
    collections::{HashMap, VecDeque},
    time::Duration,
};
use tokio::sync::{Mutex, Notify};

use crate::info;

const DEFAULT_DRIVER_URL: &str = "http://localhost:4444";
#[derive(Debug, Deserialize, Serialize)]
pub struct StatusResponse {
    pub value: Value,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Value {
    pub ready: bool,
    pub message: String,
    pub nodes: Vec<Node>,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Node {
    pub id: String,
    pub uri: String,
    pub max_sessions: u32,
    pub os_info: OsInfo,
    pub heartbeat_period: u64,
    pub availability: String,
    pub version: String,
    pub slots: Vec<Slot>,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct OsInfo {
    pub arch: String,
    pub name: String,
    pub version: String,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Slot {
    pub id: SlotId,
    pub last_started: String,
    pub session: Option<Session>,
    pub stereotype: Stereotype,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct SlotId {
    pub host_id: String,
    pub id: String,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Stereotype {
    pub browser_name: String,
    pub browser_version: String,
    #[serde(rename = "goog:chromeOptions")]
    pub chrome_options: ChromeOptions,
    pub platform_name: String,
    #[serde(rename = "se:noVncPort")]
    pub no_vnc_port: u32,
    #[serde(rename = "se:vncEnabled")]
    pub vnc_enabled: bool,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct ChromeOptions {
    pub binary: Option<String>,
    pub debugger_address: Option<String>,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Session {
    pub capabilities: Capabilities,
    pub session_id: String,
    pub start: String,
    pub stereotype: Stereotype,
    pub uri: String,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Capabilities {
    #[serde(rename = "acceptInsecureCerts")]
    pub accept_insecure_certs: bool,
    #[serde(rename = "browserName")]
    pub browser_name: String,
    #[serde(rename = "browserVersion")]
    pub browser_version: String,
    pub chrome: ChromeCapabilities,
    #[serde(rename = "networkConnectionEnabled")]
    pub network_connection_enabled: bool,
    #[serde(rename = "pageLoadStrategy")]
    pub page_load_strategy: String,
    #[serde(rename = "platformName")]
    pub platform_name: String,
    pub proxy: HashMap<String, String>,
    pub timeouts: Timeouts,
    #[serde(rename = "unhandledPromptBehavior")]
    pub unhandled_prompt_behavior: String,
    #[serde(rename = "webauthn:extension:credBlob")]
    pub cred_blob: bool,
    #[serde(rename = "webauthn:extension:largeBlob")]
    pub large_blob: bool,
    #[serde(rename = "webauthn:extension:minPinLength")]
    pub min_pin_length: bool,
    #[serde(rename = "webauthn:extension:prf")]
    pub prf: bool,
    #[serde(rename = "webauthn:virtualAuthenticators")]
    pub virtual_authenticators: bool,
    #[serde(rename = "goog:chromeOptions")]
    pub chrome_options: ChromeOptions,
    #[serde(rename = "se:bidiEnabled")]
    pub bidi_enabled: bool,
    #[serde(rename = "se:cdp")]
    pub cdp: String,
    #[serde(rename = "se:cdpVersion")]
    pub cdp_version: String,
    #[serde(rename = "se:vnc")]
    pub vnc: String,
    #[serde(rename = "se:vncEnabled")]
    pub vnc_enabled: bool,
    #[serde(rename = "se:vncLocalAddress")]
    pub vnc_local_address: String,
    #[serde(rename = "setWindowRect")]
    pub set_window_rect: bool,
    #[serde(rename = "strictFileInteractability")]
    pub strict_file_interactability: bool,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct ChromeCapabilities {
    #[serde(rename = "chromedriverVersion")]
    pub chromedriver_version: String,
    #[serde(rename = "userDataDir")]
    pub user_data_dir: String,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Timeouts {
    pub implicit: u64,
    #[serde(rename = "pageLoad")]
    pub page_load: u64,
    pub script: u64,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct CreateSessionResponse {
    pub value: SessionValue,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct SessionValue {
    #[serde(rename = "sessionId")]
    pub session_id: String,
    pub capabilities: Capabilities,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct SessionCapabilities {
    #[serde(rename = "acceptInsecureCerts")]
    pub accept_insecure_certs: bool,
    #[serde(rename = "browserName")]
    pub browser_name: String,
    #[serde(rename = "browserVersion")]
    pub browser_version: String,
    pub chrome: ChromeCapabilities,
    #[serde(rename = "fedcm:accounts")]
    pub fedcm_accounts: bool,
    #[serde(rename = "goog:chromeOptions")]
    pub chrome_options: ChromeOptions,
    #[serde(rename = "networkConnectionEnabled")]
    pub network_connection_enabled: bool,
    #[serde(rename = "pageLoadStrategy")]
    pub page_load_strategy: String,
    #[serde(rename = "platformName")]
    pub platform_name: String,
    pub proxy: HashMap<String, String>,
    #[serde(rename = "se:bidiEnabled")]
    pub bidi_enabled: bool,
    #[serde(rename = "se:cdp")]
    pub cdp: String,
    #[serde(rename = "se:cdpVersion")]
    pub cdp_version: String,
    #[serde(rename = "se:vnc")]
    pub vnc: String,
    #[serde(rename = "se:vncEnabled")]
    pub vnc_enabled: bool,
    #[serde(rename = "se:vncLocalAddress")]
    pub vnc_local_address: String,
    #[serde(rename = "setWindowRect")]
    pub set_window_rect: bool,
    #[serde(rename = "strictFileInteractability")]
    pub strict_file_interactability: bool,
    pub timeouts: Timeouts,
    #[serde(rename = "unhandledPromptBehavior")]
    pub unhandled_prompt_behavior: String,
    #[serde(rename = "webauthn:extension:credBlob")]
    pub webauthn_cred_blob: bool,
    #[serde(rename = "webauthn:extension:largeBlob")]
    pub webauthn_large_blob: bool,
    #[serde(rename = "webauthn:extension:minPinLength")]
    pub webauthn_min_pin_length: bool,
    #[serde(rename = "webauthn:extension:prf")]
    pub webauthn_prf: bool,
    #[serde(rename = "webauthn:virtualAuthenticators")]
    pub webauthn_virtual_authenticators: bool,
}

#[allow(dead_code)]
pub enum Browser {
    Chrome,
    Firefox,
}

#[derive(Debug)]
pub struct WebDriver {
    client: Client,
    driver_url: String,
}

impl Default for WebDriver {
    fn default() -> Self {
        Self {
            client: Client::builder()
                .timeout(Duration::from_secs(5))
                .build()
                .unwrap(),
            driver_url: DEFAULT_DRIVER_URL.to_string(),
        }
    }
}

impl WebDriver {
    pub fn new(driver_url: Option<String>) -> Self {
        Self {
            client: Client::builder()
                .timeout(Duration::from_secs(5))
                .build()
                .unwrap(),
            driver_url: driver_url
                .unwrap_or(String::from(DEFAULT_DRIVER_URL))
                .to_string(),
        }
    }

    pub async fn status(&self) -> Result<StatusResponse, reqwest::Error> {
        let res = self
            .client
            .get(format!("{}{}", self.driver_url, "/wd/hub/status"))
            .send()
            .await?;

        res.json::<StatusResponse>().await
    }

    pub async fn create_session(&self, browser: Browser) -> Result<String, reqwest::Error> {
        let mut headers = HeaderMap::new();
        headers.insert(USER_AGENT, HeaderValue::from_static("reqwest"));
        headers.insert(ACCEPT, HeaderValue::from_static("application/json"));

        let body = json!({
            "capabilities": {
                "alwaysMatch": {
                    "browserName": match browser {
                        Browser::Chrome => "chrome",
                        Browser::Firefox => "firefox"
                    }
                }
            }
        });

        let res = self
            .client
            .post(format!("{}{}", self.driver_url, "/wd/hub/session"))
            .headers(headers)
            .body(body.to_string())
            .send()
            .await?;

        Ok(res.json::<CreateSessionResponse>().await?.value.session_id)
    }

    pub async fn navigate_to_url(&self, session_id: &str, url: &str) -> Result<(), reqwest::Error> {
        self.client
            .post(format!(
                "{}{}{}{}",
                self.driver_url, "/wd/hub/session/", session_id, "/url"
            ))
            .body(json!({"url": url}).to_string())
            .send()
            .await?;

        Ok(())
    }

    pub async fn get_session_url_content(
        &self,
        session_id: &str,
    ) -> Result<String, reqwest::Error> {
        let res = self
            .client
            .get(format!(
                "{}{}{}{}",
                self.driver_url, "/wd/hub/session/", session_id, "/source"
            ))
            .send()
            .await?;

        let json = res.json::<serde_json::Value>().await?;
        let html = json["value"].as_str().unwrap();
        Ok(html.to_string())
    }

    pub async fn delete_session(&self, session_id: &str) -> Result<(), reqwest::Error> {
        self.client
            .delete(format!(
                "{}{}{}",
                self.driver_url, "/wd/hub/session/", session_id
            ))
            .send()
            .await?;

        Ok(())
    }
}

#[derive(Debug)]
pub struct SessionManager {
    ids: Mutex<VecDeque<String>>,
    notify: Notify,
}

impl Default for SessionManager {
    fn default() -> Self {
        Self {
            ids: Mutex::new(VecDeque::new()),
            notify: Notify::new(),
        }
    }
}

impl SessionManager {
    pub async fn push(&self, session_id: String) {
        self.ids.lock().await.push_back(session_id);
    }

    pub async fn aquire(&self) -> String {
        loop {
            let mut ids = self.ids.lock().await;

            if let Some(id) = ids.pop_front() {
                return id;
            } else {
                // I've heard there could be a deadlock condition so I'm dropping before notifying
                drop(ids);
                self.notify.notified().await;
            }
        }
    }

    pub async fn release(&self, id: String) {
        let mut ids = self.ids.lock().await;
        info!("ID {} released", &id);
        ids.push_back(id);

        self.notify.notify_one();
    }

    pub async fn take(self) -> Vec<String> {
        let mut ids = self.ids.lock().await;
        ids.drain(..).collect()
    }
}
