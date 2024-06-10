import createAuth0Client from "@auth0/auth0-spa-js";

const config = {
    domain: "poiw.eu.auth0.com",
    clientId: "FX1Is0mT4DyDfzh4MsE3xtBk4Wer7ioK",
    audience: "https://api.poiw.org"
  };

let auth = {
    async createClient() {
        let auth0Client = await createAuth0Client({
            domain: config.domain,
            client_id: config.clientId,
            audience: config.audience,
            redirect_uri: process.env.NODE_ENV == 'production' ? "https://doora.poiw.org/login" :`https://${window.location.hostname}:3000/login`,
            cacheLocation: 'localstorage',
            scope: "openid email"
        });

        return auth0Client;
    },
    async login(client) {
        await client.loginWithRedirect();
    },
    async logout() {
        return (await this.createClient()).logout({
            returnTo: "https://doora.poiw.org"
        });
    },
    async checkSession() {

    },
    async isAuthenticated() {
        return (await this.createClient()).isAuthenticated()
    },
    async getUser() {
        return (await this.createClient()).getUser()
    },
    async getToken(){
        return (await this.createClient()).getTokenSilently()
    }
}

export default auth
