const core = require("@actions/core");
const actions_exec = require("@actions/exec");
const { Octokit } = require("octokit");

async function exec(command, args, input = "") {
    let output = "";

    await actions_exec
        .getExecOutput(command, args, {
            ignoreReturnCode: true,
            input: Buffer.from(input),
        })
        .then((result) => {
            if (result.exitCode != 0) {
                throw result.stderr.trim();
            }

            output = result.stdout;
        });

    return output;
}

async function get_versions(octokit, username, imageName) {
    try {
        core.info(`Retrieving ${username}/${imageName} versions...`);
        return await octokit.request(`GET /users/${username}/packages/container/${imageName}/versions`)["data"];
    } catch {
        return [];
    }
}

function image_base_name(name) {
    return name.split("/")[-1];
}

function is_pr() {
    return process.env.GITHUB_REF.startsWith("refs/pull/");
}

function is_main_branch() {
    return process.env.GITHUB_REF == "refs/heads/main";
}

function image_version(versions, tag) {
    core.info(`Searching for image with tag ${tag}...`);

    for (const version of versions) {
        for (const versionTag of version["metadata"]["container"]["tags"]) {
            if (versionTag == tag) {
                return version["id"];
            }
        }
    }

    return "";
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
        const containerFileCommit = await exec("git", ["log", "-n", "1", "--pretty=format:%h", "--", containerFile]);
        const image = `${repository}/${username}/${imageName}:${containerFileCommit}`;

        if (image_version(versions, containerFileCommit) == "") {
            await exec("docker", ["login", repository, "--username", username, "--password-stdin"], token);
            await exec("docker", ["build", "-f", containerFile, "-t", image, containerPath]);
            await exec("docker", ["push", image]);

            if (is_main_branch()) {
                const imageLatest = `${repository}/${username}/${imageName}:latest`;
                const latestCommitImageVersion = image_version(versions, "latest");
                await exec("docker", ["tag", image, imageLatest]);
                await exec("docker", ["push", imageLatest]);

                if (latestCommitImageVersion != "") {
                    core.info(`Deleting previous latest image (package version ${latestCommitImageVersion})`);
                    await octokit.request(`DELETE /users/${username}/packages/container/${imageName}/versions/${latestCommitImageVersion}`);
                }
            } else if (is_pr()) {
                const pr = pr_name();
                const imagePR = `${repository}/${username}/${imageName}:pr${pr}`;
                await exec("docker", ["tag", image, imagePR]);
                await exec("docker", ["push", imagePR]);
            }

            core.setOutput("created", "true");
        }

        core.setOutput("image", image);
    } catch (error) {
        core.setFailed(error);
    }
}

run();
