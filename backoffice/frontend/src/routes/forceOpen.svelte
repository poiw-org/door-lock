<script context="module" lang="ts">
	export const prerender = false;
</script>
<script lang="ts">
    import Fa from 'svelte-fa'
	import { faDoorOpen, faDoorClosed } from '@fortawesome/free-solid-svg-icons'
    import API from "../lib/api";
    import auth from '$lib/auth/authService';

    let forceUnlock = async () => {

        await API.get(`/forceOpen`)

        alert("Force unlock command sent!");
        window.location = "/";
    }

    let cancel = () => {
        window.location = "/";
    }

</script>

<span class="p-4 flex flex-col">
    <span>Are you sure you want to send a force unlock command?</span>
    <span class="flex items-center gap-4">
        <button on:click={forceUnlock} class="mt-10 p-4 border border-green-700 text-green-700 flex items-center gap-2"><Fa icon={faDoorOpen}/>YES</button>
        <button on:click={cancel} class="mt-10 p-4 border border-red-700 text-red-700 flex items-center gap-2"><Fa icon={faDoorClosed}/>CANCEL</button>
    </span>
</span>
