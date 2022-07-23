import {Collection, EntityConstructorOrPath, getRepository, ISubCollection, SubCollection} from 'fireorm';
import Key from "./key";

@Collection()
export default class User {
    id: string;
    fullName: string;
    authorizationLevel: number;

    static getRepo = () => getRepository(User as EntityConstructorOrPath<any>);

    @SubCollection(Key)
    keys?: ISubCollection<Key>;

}