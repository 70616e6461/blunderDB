// Cynhyrchwyd y ffeil hon yn awtomatig. PEIDIWCH Â MODIWL
// This file is automatically generated. DO NOT EDIT
import {main} from '../models';

export function CheckDatabaseVersion():Promise<string>;

export function CheckVersion(arg1:string):Promise<void>;

export function ClearCommandHistory():Promise<void>;

export function DeleteAnalysis(arg1:number):Promise<void>;

export function DeleteComment(arg1:number):Promise<void>;

export function DeletePosition(arg1:number):Promise<void>;

export function GetDatabaseVersion():Promise<string>;

export function LoadAllPositions():Promise<Array<main.Position>>;

export function LoadAnalysis(arg1:number):Promise<main.PositionAnalysis>;

export function LoadCommandHistory():Promise<Array<string>>;

export function LoadComment(arg1:number):Promise<string>;

export function LoadMetadata():Promise<{[key: string]: string}>;

export function LoadPosition(arg1:number):Promise<main.Position>;

export function LoadPositionsByFilters(arg1:main.Position,arg2:boolean,arg3:boolean,arg4:string,arg5:string,arg6:string,arg7:string,arg8:string,arg9:string,arg10:string,arg11:string,arg12:string,arg13:string,arg14:string,arg15:string,arg16:string,arg17:string,arg18:string,arg19:string,arg20:boolean,arg21:boolean,arg22:string,arg23:string,arg24:string,arg25:string):Promise<Array<main.Position>>;

export function Migrate_1_0_0_to_1_1_0():Promise<void>;

export function OpenDatabase(arg1:string):Promise<void>;

export function PositionExists(arg1:main.Position):Promise<{[key: string]: any}>;

export function SaveAnalysis(arg1:number,arg2:main.PositionAnalysis):Promise<void>;

export function SaveCommand(arg1:string):Promise<void>;

export function SaveComment(arg1:number,arg2:string):Promise<void>;

export function SaveMetadata(arg1:{[key: string]: string}):Promise<void>;

export function SavePosition(arg1:main.Position):Promise<number>;

export function SetupDatabase(arg1:string):Promise<void>;

export function UpdatePosition(arg1:main.Position):Promise<void>;
