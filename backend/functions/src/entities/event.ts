import {Collection, EntityConstructorOrPath, getRepository} from 'fireorm';
import {EventTypes} from "../enums/eventTypes";

@Collection()
export default class Event {
    declare id: string;
    declare type: EventTypes;
    declare timestamp: number;
    declare payload: any;

    static getRepo = () =>  getRepository(Event as EntityConstructorOrPath<any>);
}