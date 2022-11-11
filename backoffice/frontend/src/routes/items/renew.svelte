<script context="module" lang="ts">
	export const prerender = false;
</script>
<script lang="ts">
    import {onMount} from "svelte"
    import auth from "../../lib/auth/authService"
    import API from "../../lib/api"
    import Loading from "../../lib/loading.svelte"
    import Fa from 'svelte-fa'
	import { faPencilAlt, faHandSparkles } from '@fortawesome/free-solid-svg-icons'
    import { page } from "$app/stores";
    import Key from "../../lib/entities/key"
  import { DateInput } from "date-picker-svelte";

    let key: Key;
    let id

    let processing = true

    onMount(async ()=>{
        processing = true;
        id = $page.query.get("id")
        if(!(await auth.isAuthenticated())) window.location = "/login"


         key = await Key.getById(id)

        processing = false;
    })

    let submit = async () => {
        processing = true
        await key.renew();
        window.location = `/items?id=${key.id}`;

    }
</script>

{#if !processing}
<form on:submit={submit} class="px-4 md:px-10 py-4 flex flex-col items-start justify-start">
    <p class="text-xl font-bold">Renew {key.id} owned by {key.user}</p>
    <span class="flex gap-4 mt-10">
        <span class="flex flex-col gap-2">
            <label>Activate not before</label>
            <DateInput format="yyyy-MM-dd" min={new Date()} max={new Date("2100")} bind:value={key.notBefore} />
        </span>
        <span class="flex flex-col gap-2">
            <label>Expires at</label>
            <DateInput format="yyyy-MM-dd" min={new Date()} max={new Date("2100")} bind:value={key.expiresAt} />
        </span>
    </span>
    <button type="submit" class="mt-10 p-4 border border-green-700 text-green-700 flex items-center gap-2"><Fa icon={faHandSparkles}/>RENEW</button>
</form>
{:else}
<span class="flex pb-20"><Loading/></span>

{/if}
