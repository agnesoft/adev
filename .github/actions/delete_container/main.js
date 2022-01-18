const core = require("@actions/core");
const { Octokit } = require("octokit");

function extract_image_name(image) {
    const ar = image.split("/");

    if (ar.length == 3) {
        return ar[2].split(":")[0];
    }

    if (ar.length == 2) {
        return ar[1].split(":")[0];
    }

    throw `Invalid image name '${image}'`;
}

function extract_image_tag(image) {
    const ar = image.split(":");

    if (ar.length == 2) {
        return ar[1];
    }

    throw `Invalid image name '${image}'`;
}

function extract_username(image) {
    const ar = image.split("/");

    if (ar.length == 3) {
        return ar[1];
    }

    if (ar.length == 2) {
        return ar[0];
    }

    throw `Invalid image name '${image}'`;
}

function image_id(versions, tag) {
    for (const version of versions) {
        for (const versionTag of version["metadata"]["container"]["tags"]) {
            if (versionTag == tag) {
                return version["id"];
            }
        }
    }

    throw "Image 'image' not found.";
}

async function run() {
    try {
        const image = core.getInput("image");
        const username = extract_username(image);
        const imageName = extract_image_name(image);
        const tag = extract_image_tag(image);
        const token = core.getInput("token");
        const octokit = new Octokit({ auth: token });
        const versions = await octokit.request(`GET /users/${username}/packages/container/${imageName}/versions`);
        const id = image_id(versions["data"], tag);

        core.info(`Deleting ${image} (package version ${id})`);
        await octokit.request(`DELETE /users/${username}/packages/container/${imageName}/versions/${id}`);
    } catch (error) {
        core.setFailed(error);
    }
}

run();
