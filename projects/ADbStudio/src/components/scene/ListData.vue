<template>
    <ul class="list-data">
        <li v-for="element in data" :key="element.id">
            <Element :element="element" v-on="$listeners" />
        </li>
    </ul>
  
</template>

<script>
import Element from "./Element";

export default {
    name: "ListData",
    components: {
        Element
    },
    props: {
        data: Array,
    },
}
</script>

<style scoped>
    .list-data>li{
        float: left;
        margin: 0.3rem;
    }
    .element{
        background: var(--light-color);
        padding: 0.5rem 1rem ;
        transition: background-color 0.4s ease;
        cursor: pointer;
    }
    .element:hover{
        background: var(--light-primary-color);
    }

</style>
