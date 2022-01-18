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

async function get_versions(octokit, username, imageName) {
    try {
        return await octokit.request(`GET /users/${username}/packages/container/${imageName}/versions`);
    } catch (error) {
        let versions = {};
        versions["data"] = [];
        return versions;
    }
}

function image_base_name(name) {
    const ar = name.split("/");
    return ar[ar.length - 1];
}

function is_pr() {
    return process.env.GITHUB_REF.startsWith("refs/pull/");
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

async function login(repository, username, token) {
    const loginArgs = ["login", repository, "--username", username, "--password-stdin"];

    core.info(`Logging into ${repository}...`);

    await github_exec
        .getExecOutput("docker", loginArgs, {
            ignoreReturnCode: true,
            silent: true,
            input: Buffer.from(token),
        })
        .then((res) => {
            if (res.stderr.length > 0 && res.exitCode != 0) {
                throw res.stderr.trim();
            }

            core.info(`Login Succeeded!`);
        });
}

function pr_name() {
    return process.env.GITHUB_REF.split("/")[2];
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
        const versions = await get_versions(octokit, username, imageName);
        const containerFileCommit = await last_file_commit(containerFile);
        const image = `${repository}/${username}/${imageName}:${containerFileCommit}`;
        const imageLatest = `${repository}/${username}/${imageName}:latest`;

        if (!image_version_exists(versions["data"], containerFileCommit)) {
            await login(repository, username, token);
            await exec(`docker build -f ${containerFile} -t ${image} ${containerPath}`);
            await exec(`docker push ${image}`);

            if (is_main_branch()) {
                const latestCommitImageId = await latest_image_commit_id(versions["data"]);
                await exec(`docker tag ${image} ${imageLatest}`);
                await exec(`docker push ${imageLatest}`);

                if (latestCommitImageId != "") {
                    await octokit.request(`DELETE /users/${username}/packages/container/${imageName}/versions/${latestCommitImageId}`);
                }
            } else if (is_pr()) {
                const pr = pr_name();
                const imagePR = `${repository}/${username}/${imageName}:${pr}`;
                await exec(`docker tag ${image} ${imagePR}`);
                await exec(`docker push ${imagePR}`);
            }

            core.setOutput("created", true);
        }

        core.setOutput("image", image);
    } catch (error) {
        core.setFailed(error);
    }
}

run();
