import API from "../api";
import auth from "../auth/authService";

export default class Key {
    declare id: string;
    declare user: {
        id: string;
        email: string;
    };
    declare serialNumber: string;
    declare expiresAt: Date;
    declare issuedAt: Date;
    declare notBefore: Date;
    declare issuedBy: string;
    declare disabled: boolean;
    declare jwt: string;


    constructor(id: string, user: any,  serialNumber: string, expiresAt: Date, notBefore: Date, issuedAt?:Date, issuedBy?: string, disabled?: boolean, jwt?: string) {
        this.id = id;
        this.user = user;
        this.serialNumber = serialNumber;
        this.expiresAt = expiresAt;
        this.issuedAt = issuedAt;
        this.issuedBy = issuedBy;
        this.jwt = jwt;
        this.notBefore = notBefore;
        this.issuedBy = issuedBy;
        this.disabled = disabled;

    }

    public async create(): Promise<void>{
        return new Promise<void>(async () =>{
            await API.post("/key",{
                id: this.id,
                user: this.user,
                serialNumber: this.serialNumber,
                expiresAt: this.expiresAt,
                notBefore: this.notBefore
            })
        })
    }

    public async renew(): Promise<void>{
            await API.patch("/key/renew",{
                id: this.id,
                expiresAt: this.expiresAt,
                notBefore: this.notBefore
            })
    }

    public async disable(): Promise<void>{
        await API.patch("/key/disable",{
            id: this.id
        })
    }

    public async enable(): Promise<void>{
        await API.patch("/key/enable",{
            id: this.id
        })
    }

    public async delete(): Promise<void>{
        await API.delete(`/key?id=${this.id}`)
    }

    public static async getById(_id: string): Promise<Key | undefined> {
            try{
                let {data} = await API.get(`/key?id=${_id}`)
                return Key.fromJSON(data)
            }catch(e){
                console.log(e.message);
                return undefined
            }

    }

    public static async getBySerialNumber(serialNumber: string): Promise<Key | undefined> {
        try{
            let {data} = await API.get(`/key?serialNumber=${serialNumber}`)
            return Key.fromJSON(data)
        }catch(e){
            console.log(e.message);
            return undefined
        }

}

    // public toJSON(): object {
    //     console.log(this)
    //     return{
    //         _id: this._id,
    //         title: this.title,
    //         description: this.description,
    //         shelf: this.shelf,
    //         checkIn: this.checkIn,
    //         checkOut: this.checkOut,
    //         editedBy: this.editedBy,
    //         barcode: this.barcode
    //     }
    // }

    static fromJSON(key: any): Key{
        let {id, user,  serialNumber, expiresAt, notBefore, issuedAt, issuedBy, jwt, disabled} = key;
        expiresAt = new Date(expiresAt);
        issuedAt = new Date(issuedAt);
        notBefore = new Date(notBefore);

        return new Key(id, user,  serialNumber, expiresAt, notBefore, issuedAt, issuedBy, disabled, jwt)
    }

    // public static async  search(term: string): Promise<Item[]>{
    //     let {data} = await API.get("/items",{term})
    //     return data.map((item: any)=>{
    //         let {_id, title, description, shelf, checkIn, checkOut, editedBy} = item
    //         new Item(_id, title, description, shelf, checkIn, checkOut, editedBy)
    //     })
    // }
}