import axios from "axios"
const domain = process.env.NODE_ENV == 'production' ? 'https://api.poiw.org/doora' : `http://${window.location.hostname}:3001/doora`

export default {

    async get(endpoint: string, config?: any): Promise<any> {
        return await axios.get(domain+endpoint, config)
    },

    async post(endpoint: string, params?: any, config?: any): Promise<any> {
        return await axios.post(domain+endpoint, params, config)
    },

    async patch(endpoint: string, params?: any, config?: any): Promise<any> {
        return await axios.patch(domain+endpoint, params,config)
    },
    async delete(endpoint: string, config?: any): Promise<any> {
        return await axios.delete(domain+endpoint, config)
    },
    async put(endpoint: string, params?: any, config?: any): Promise<any> {
        return await axios.put(domain+endpoint, params)
    }
}