<script lang="ts">
import { browser, dev } from '$app/env';

export const hydrate = dev;
export const router = browser;
export const prerender = false;

import { page } from "$app/stores";

import Key from "../../lib/entities/key"
import Loading from "../../lib/loading.svelte"
import auth from "../../lib/auth/authService"
import {onMount} from "svelte"
import API from "../../lib/api";
import { Entry, EntryType } from '$lib/entities/entry';
import Fa from 'svelte-fa'
import { faRedo, faHandSparkles, faFire, faStopCircle, faArrowAltCircleLeft, faTrash, faCheck, faCheckCircle} from '@fortawesome/free-solid-svg-icons'

let key: Key;
let processing = true;
let status = "";

let id = $page.query.get("id")
let checkOut = async () => {
        if ('NDEFReader' in window){
            //@ts-ignore
            const ndef = new NDEFReader();

            processing = true;
            status = `Please approach the tag with S/N ${key.serialNumber}`

            ndef.scan()
            .then(() => {
                ndef.onreadingerror = () => {
                    processing = false;
                    alert("Cannot read data from the NFC tag. Try another one?");
                    location.reload();

                };
                ndef.onreading = async (event) => {
                    status = `Validating S/N`
                    if(key.serialNumber != event.serialNumber){
                        processing = false;
                        alert("The tag's serial number has changed. Please re-issue the key.");
                        location.reload();
                        return;

                    }
                    status = `Writing key ${key.id}`
                    await ndef.write(key.jwt);

                    processing = false;

                    alert("Success! 😁");
                    location.reload();
                };
            }).catch(error => {
                processing = false;
                alert(`Error! Scan failed to start: ${error}.`);

            });

        }else{
            alert("This feature is only available in Chrome Android. Please switch your browser or write the token manually.")

        }
}

let disableKey = async () => {
    if (confirm('Are you sure you want to DISABLE this key?')) {
        await key.disable();
        location.reload();
    }
}

let enableKey = async () => {
    if (confirm('Are you sure you want to ENABLE this key?')) {
        await key.enable();
        location.reload();
    }
}

let deleteKey = async () => {
    if (confirm('Are you sure you want to DELETE this key?')) {
        if (confirm('Deleting a key will NOT disable it. That means it could still be used to unlock a door, since it\'s it works offline cryptographically. Are you ABSOLUTELY sure?')) {
            await key.delete();
            window.location = "/";
        }
    }
}

let goToRenew = () => {
    window.location=`/items/renew?id=${key.id}`
}

let entries;

onMount(async ()=>{
    if(!(await auth.isAuthenticated())) window.location = "/login"
    id = $page.query.get("id")
    key = await Key.getById(id)
    processing = false;


    let {data} = await API.get(`/entries?id=${key.id}`, {
                    headers: {
                        Authorization: `Bearer ${await auth.getToken()}`
                    }
                })

    entries = Entry.fromJSON(data);
})

let selectedEntry: Entry = undefined;
let showEntry = (entry: Entry) => {
    if(selectedEntry == entry) return selectedEntry = undefined;
    selectedEntry = entry;
}
</script>

<div class="px-4 md:px-10 py-4">
    {#if processing}
        <span class="w-full text-center">{status}</span>
        <Loading/>
    {:else}

        <div class="flex justify-start items-center gap-5">
            <span class="text-xl">{key.id}</span>
            {#if key.disabled}
                <span class="border p-3 border-red-500 text-red-500">DISABLED</span>
                {:else}
                <span class="border p-3 border-green-500 text-green-500">ENABLED</span>
            {/if}
        </div>
        <div class="flex pt-10 flex-col gap-4">
            <span class="flex flex-col">
                <span class="text-sm font-light">USER</span>
                <span  class="font-bold text-lg">{key.user.email}</span>
            </span>
            <span class="flex flex-col">
                <span class="text-sm font-light">TAG S/N</span>
                <span  class="font-bold text-lg">{key.serialNumber || "-"}</span>
            </span>
            <span class="flex flex-col">
                <span class="text-sm font-light">VALID NOT BEFORE</span>
                <span  class="font-bold text-lg">{key.notBefore ? new Date(key.notBefore).toLocaleString("el-GR") : "-"}</span>
            </span>
            <span class="flex flex-col">
                <span class="text-sm font-light">EXPIRES AT</span>
                <span  class="font-bold text-lg">{new Date(key.expiresAt).toLocaleString("el-GR")}</span>
            </span>
            <span class="flex flex-col">
                <span class="text-sm font-light">ISSUED AT</span>
                <span  class="font-bold text-lg">{new Date(key.issuedAt).toLocaleString("el-GR")}</span>
            </span>
            <span class="flex flex-col">
                <span class="text-sm font-light">ISSUED BY</span>
                <span  class="font-bold text-lg">{key.issuedBy}</span>
            </span>
            <span class="flex flex-col">
                <span class="text-sm font-light">JWT</span>
                <textarea disabled>{key.jwt}</textarea>
            </span>
        </div>
            <div class="flex flex-col md:flex-row gap-4 pt-20">
                <button on:click={goToRenew} class="border p-3 border-blue-500 text-blue-500 flex gap-2 items-center"><Fa icon={faHandSparkles}/>RENEW</button>
                <button on:click={checkOut} class="border p-3 border-yellow-500 text-yellow-500 flex gap-2 items-center"><Fa icon={faFire}/>WRITE TO TAG</button>
                {#if key.disabled}
                    <button on:click={enableKey} class="border p-3 border-purple-500 text-purple-500 flex gap-2 items-center"><Fa icon={faCheckCircle}/>ENABLE</button>
                {:else}
                    <button on:click={disableKey} class="border p-3 border-purple-500 text-purple-500 flex gap-2 items-center"><Fa icon={faStopCircle}/>DISABLE</button>
                {/if}
                <button on:click={deleteKey} class="border p-3 border-red-500 text-red-500 flex gap-2 items-center"><Fa icon={faTrash}/>DELETE</button>

            </div>
    {/if}

    {#if entries}
    <div class="flex flex-col gap-2 py-4">
        <span>Logs:</span>
        {#each entries as entry}
            <div on:click={()=>showEntry(entry)} class="bg-gray-50 flex justify-between items-center rounded-sm p-2 cursor-pointer">
                <span class="bold">{entry.getDateTimeString()}</span>
                {#if entry.type == EntryType.SUCCESSFUL} <span class="text-green-400"><Fa icon={faCheckCircle}/></span>{/if}
                {#if entry.type == EntryType.FAILED} <span class="text-red-400"><Fa icon={faStopCircle}/></span>{/if}
            </div>
            {#if selectedEntry == entry}
            <div class="flex flex-col p-2">
                <div>Occured on: {entry.getDateTimeString()}</div>
                <div>Outcome: <span class={`${entry.type == EntryType.SUCCESSFUL ? "text-green-400" : "text-red-400"}`}>{entry.type == EntryType.SUCCESSFUL ? "SUCCESS" : "FAILURE"}</span></div>
                {#if entry.type == EntryType.FAILED}
                    <div>Reason: {entry.reason}</div>
                {/if}
                {#if entry.metadata.serialNumberFromTag}
                <div>Serial Number from Tag: {entry.metadata.serialNumberFromTag}</div>
            {/if}
            </div>
            {/if}
        {/each}
    </div>
    {/if}
</div>

