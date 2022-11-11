<script context="module" lang="ts">
	export const prerender = false;
</script>
<script>
    import {onMount} from "svelte"
    import auth from "../../lib/auth/authService"
    import API from "../../lib/api"
    import Loading from "../../lib/loading.svelte"
    import Fa from 'svelte-fa'
	import { faKey, faWifi, faPlus} from '@fortawesome/free-solid-svg-icons'
    import { DateInput } from 'date-picker-svelte'
    import Select from 'svelte-select';

    let key ={
        user: {},
        serialNumber: "",
        expiresAt: new Date(),
        notBefore: new Date(),
    }

    let processing = false


    let submit = async () => {
        processing = true
        let token = await auth.getToken()
        try{
            let {data:{id}} = await API.post("/key",
                key,
                {
                    headers: {
                        'Authorization': 'Bearer ' + token
                    }
                }
            )
        window.location = `/items?id=${id}`
        }catch(e){
            alert("An error occured while communicating with the backend server")
        }

        processing = false;

    }

    let getSerialNumber = async () => {
        if ('NDEFReader' in window){
            //@ts-ignore
            const ndef = new NDEFReader();

            ndef.scan().then(() => {
                key.serialNumber = "waiting for tag..."

                ndef.onreadingerror = () => {
                    alert("Cannot read data from the NFC tag. Try another one?");
                    key.serialNumber = ""
                };
                ndef.onreading = event => {
                    key.serialNumber = event.serialNumber
                };
            }).catch(error => {
                alert(`Error! Scan failed to start: ${error}.`);
            });
        }else{
            alert("This feature is only available in Chrome Android. Please switch your browser or input the S/N manually.")

        }

    }

    let users;
    onMount(async ()=>{
        if(!(await auth.isAuthenticated())) window.location = "/login";

        users = (await API.get(`/users`, {
                    headers: {
                        Authorization: `Bearer ${await auth.getToken()}`
                    }
                })).data.map(user=>{
                    return {
                        value: user.id,
                        label: user.email
                    }
                });
    })

    function handleSelectUser(event) {
        key.user = {
            email: event.detail.label,
            id: event.detail.value
        };
    }
</script>

{#if !processing && users}
<form on:submit={submit} class="px-4 md:px-10 py-10 flex flex-col gap-10 items-start justify-start">
    <span class="text-xl w-full"><Select on:select={handleSelectUser} items={users}></Select></span>
    <span class="flex gap-4">
        <span class="flex flex-col justify-center">
            <span class="flex items-center w-full"><span class="text-xl"><Fa icon={faKey}/></span><input required placeholder="Tag S/N" type="tel" bind:value={key.serialNumber} maxlength="12" class="p-4 w-full text-xl text-teal-700"/></span>
            {#if !key.serialNumber}
            <span on:click={getSerialNumber} class="cursor-pointer text-teal-700 flex items-center gap-2"><Fa icon={faWifi}/>Scan tag</span>
            {:else if key.serialNumber != "waiting for tag..."}
            <span class="px-7 text-xs">The key will be boud to this S/N. You will not be able to write the key to another tag.</span>
            {/if}
        </span>
    </span>
    <span class="flex gap-4">
        <span class="flex flex-col gap-2">
            <label>Activate not before</label>
            <DateInput format="yyyy-MM-dd" min={new Date()} max={new Date("2100")} bind:value={key.notBefore} />
        </span>
        <span class="flex flex-col gap-2">
            <label>Expires at</label>
            <DateInput format="yyyy-MM-dd" min={new Date()} max={new Date("2100")} bind:value={key.expiresAt} />
        </span>
    </span>

    <button type="submit" class="mt-10 p-4 border border-teal-700 text-teal-700 flex items-center gap-2"><Fa icon={faPlus}/>CREATE</button>
</form>
{:else}
<span class="flex pb-20"><Loading/></span>

{/if}