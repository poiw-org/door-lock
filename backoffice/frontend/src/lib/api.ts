import axios from "axios"
import authService from "$lib/auth/authService";
const domain = process.env.NODE_ENV == 'production' ? 'https://api.poiw.org/doora' : `http://${window.location.hostname}:3001/doora`

axios.interceptors.request.use(
    config => {
        config.baseURL = domain;
        const token = authService.getToken();
        if (token) {
            config.headers.Authorization = `Bearer ${token}`
        }
        return config
    },
    error => {
        return Promise.reject(error)
    }
)

axios.interceptors.response.use(
    response => {
        return response
    },
    error => {
        if (error.response.status === 401) {
            alert('Your session has expired or you are not authorized to view this page.')
            authService.logout()
        }
        return Promise.reject(error)
    }
)

export default axios