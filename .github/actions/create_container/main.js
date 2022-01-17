const core = require("@actions/core");
const github_exec = require("@actions/exec");
const { Octokit } = require("octokit");

async function exec(command) {
    let output = "";
    let error = "";

    const options = {};
    options.listeners = {
        stdout: (data) => {
            output += data.toString();
        },
        stderr: (data) => {
            error += data.toString();
        },
    };

    const retCode = await github_exec.exec(command, [], options);

    if (retCode != 0) {
        throw error;
    }

    return output;
}

function image_base_name(name) {
    const ar = name.split("/");
    return ar[ar.length - 1];
}

function is_main_branch() {
    return process.env.GITHUB_REF == "refs/heads/main";
}

function image_version_exists(versions, tag) {
    for (const version of versions) {
        for (const versionTag of version["metadata"]["container"]["tags"]) {
            if (versionTag == tag) {
                return true;
            }
        }
    }

    return false;
}

async function last_file_commit(file) {
    return await exec(`git log -n 1 --pretty=format:%h -- ${file}`);
}

function latest_image_commit_id(versions) {
    for (const version of versions) {
        for (const versionTag of version["metadata"]["container"]["tags"]) {
            if (versionTag == "latest") {
                return version["id"];
            }
        }
    }

    return "";
}

async function run() {
    try {
        const username = core.getInput("username");
        const repository = "ghcr.io";
        const imageName = image_base_name(core.getInput("name"));
        const containerFile = core.getInput("containerFile");
        const containerPath = ".";
        const token = core.getInput("token");

        const octokit = new Octokit({ auth: token });
        const versions = await octokit.request(`GET /users/${username}/packages/container/${imageName}/versions`);
        const containerFileCommit = await last_file_commit(containerFile);
        const image = `${repository}/${username}/${imageName}:${containerFileCommit}`;
        const imageLatest = `${repository}/${username}/${imageName}:latest`;

        if (!image_version_exists(versions["data"], containerFileCommit)) {
            await exec(`echo ${token} | docker login ${repository} -u ${username} -password-stdin`);
            await exec(`docker build -f ${containerFile} -t ${image} ${containerPath}`);
            await exec(`docker push ${image}`);

            if (is_main_branch()) {
                const latestCommitImageId = await latest_image_commit_id(versions["data"]);
                await exec(`docker tag ${image} ${imageLatest}`);
                await exec(`docker push ${imageLatest}`);

                if (latestCommitImageId != "") {
                    await octokit.request(`DELETE /users/${username}/packages/container/${imageName}/versions/${latestCommitImageId}`);
                }
            }
        }

        core.setOutput("image", image);
    } catch (error) {
        core.setFailed(error);
    }
}

run();
