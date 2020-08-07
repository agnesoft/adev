<template>
    <ul class="list-data">
        <li class="element" v-for="elem in data" :key="elem.id">
            {{elem.id}}
        </li>
    </ul>
  
</template>

<script>
export default {
    name: "ListData",
    props: {
        data: Array,
    }
}
</script>

<style scoped>
    .element{
        background: var(--light-color);
        padding: 0.5rem 1rem ;
        transition: background-color 0.4s ease;
        margin: 0.3rem;
        cursor: pointer;
        float: left;
    }
    .element:hover{
        background: var(--light-primary-color);
    }

</style>
