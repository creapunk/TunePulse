# Workflow

This document is a preliminary contribution guidelines. It is subject to change. Requies input from other contributors.

Written by arsenier (Mr. Foton'ыч).

## Working with git

### Branches

1. There are the following types of branches: master (main), develop, simple branch and feature branch
2. master contains verified, i.e. release code. Only this code gets release tags and is expected to work with no additional modifications
3. develop contains the pre-release code. This code can contain bugs and unexpected behaviour and should be checked before merging into master
4. A simple branch is designed to implement a single task. If the task is not included in the feature branch, then it must have an entry in the changelog
5. feature-branch - a branch designed for isolated execution of large-scale work that will be performed in separate simple branches. When all work is completed, the feature branch is merged into develop. At the same time, the changelog entry is made one for the entire feature branch.

### Branch names

1. Basic: master (main), develop
2. The names of other branches consist of the task number (issue number if exists) or blank space (if there are no issue created. only for early development. TODO: mandate to create issue for the task) and the text that conveys the essence of the work performed in English, separated by a “_”sign. For example: `1111_improve_measurement_current_sense`, `12312_ﬁx_volage_control`, `_add_debug_logs_for_pwm`.
3. Branches created within the same task have the same prefix. For example: `1234_some_new_module_interface`, which describes the interface of the new module and then approves it for review, `1234_some_new_module`, which implements the previously agreed interface
4. Identical names are not allowed

### Creating a pull request on GitHub

1. Where to merge:
    1. In develop, then when tested on real hardware and successfully performed into master
    2. feature-branches go to develop. Simple branches created within the feature branch go to feature branch
    3. A simple branch goes to develop
2. Name of the request:
    1. It consists of the task number, which is a prefix, and the name in english. For example: `1234 Interface of a new module`
    2. A separate task is created for the request to transfer changes from develoto master.
    3. A request can have a short description, if it clarifies and helps, and does not clutter

### Performing a review

> May require testing in GitHub and changes based on it. Source material relied on GitLab features

1. The request author can either agree with the submitted comment/suggestion or not. In the first case, they make corrections and mark the discussion as completed (with text, a thumb - up smiley face, etc.). In the second case, they must make arguments against it. Ignoring comments/suggestions is not allowed
2. The author of the comment/suggestion may agree with the correction after seeing it, and then they should close the discussion. Otherwise, it is reasonable to say what he does not agree with
3. All comments/suggestions should be justified (by the standard, coding style, or performance/resource usage)

### Completing the request

1. To perform a merge, the request must:
    1. have resolved discussions, if any
    2. build without errors (except for simple branches created within the feature branch)
    3. have an aprove from all those who performed the review (at least one contributor)
2. The request may be closed due to its non-relevance or non-use. You can't delete requests
3. Only trusted contributors can merge requests into develop and master

### Issuing the release

1. After merging into master create corresponding tag and publish it:
```bash
git tag -a <corresponding tag> -m ""
git push --tags
```
Example:
```bash
git tag -a v1.2 -m ""
git push --tags
```

## Versioning

1. Version is defined by major and minor versions: `v<major>.<minor>`. For example: `v2.4`.
1. The minor version is incremented when release is required.
2. The reason for incrementing the major version is:
    1. loss of backward compatibility
    2. introduction of features that change the main characteristics of the product

<!-- ## Changelog

1. Needed for the story
2. At the beginning of the file - the most recent changes, at the end - the very first. In other words, new entries are added to the beginning of the file. -->
