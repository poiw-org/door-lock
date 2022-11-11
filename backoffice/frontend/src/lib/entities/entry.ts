export enum EntryType {
    FAILED = "failed",
    SUCCESSFUL = "successful"
}

export enum FailReason {
    UNKNOWN = "Unknown",
    KEY_EXPIRED = "KeyExpired",
    KEY_BLACKLISTED ="KeyBlacklisted",
    KEY_NOT_YET_ACTIVATED ="KeyNotYetActivated",
    ISSUING_DATE_IN_THE_FUTURE ="IssuingDateInTheFuture",
    SERIAL_NUMBER_MISMATCH = "SerialNumberMismatch"
}

export class Entry {
    public type: EntryType;
    public reason: FailReason | undefined;
    public key: string;
    timestamp: Date;

    public metadata:{
        serialNumberFromTag?: string;
    }

    constructor(type: EntryType, reason: FailReason, key: string, timestamp: Date, metadata: any){
        this.type = type;
        this.reason = reason;
        this.key = key;
        this.timestamp = timestamp;
        this.metadata = metadata;
    };


    public getDateTimeString(){
        let date = new Date(this.timestamp);

        return `${date.toLocaleDateString("el-GR")} ${date.toLocaleTimeString("el-GR")}`
    }

    public static fromJSON(json: any): Entry[] {
        return json.map(({type, reason, key, timestamp, metadata}) => new Entry(type, reason, key, timestamp, metadata))
    }

}