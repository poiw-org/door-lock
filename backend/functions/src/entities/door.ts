import {Collection, EntityConstructorOrPath, getRepository} from 'fireorm';
import {DoorStates} from "../enums/doorStates";

@Collection()
export default class Door {
    id: string;
    name: string;
    authorizationLevel: number;
    state: DoorStates

    static async create(name: string, authorizationLevel: number) {
        let door = new Door();
        door.name = name;
        door.authorizationLevel = authorizationLevel;
        door.state = DoorStates.UNKNOWN;

        let repo = await Door.getRepo();

        await repo.create(door);
    }

    static getRepo = () => getRepository(Door as EntityConstructorOrPath<any>);
}