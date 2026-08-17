import posixpath
import shutil
from pathlib import Path

from pygments.lexers import _mapping

from arkoi_lexer import ArkoiLexer

_mapping.LEXERS[ArkoiLexer.__name__] = (
    "arkoi_lexer",
    ArkoiLexer.name,
    tuple(ArkoiLexer.aliases),
    tuple(ArkoiLexer.filenames),
    tuple(ArkoiLexer.mimetypes),
)


def _insert_after_title(markdown: str, notice: str) -> str:
    """Insert a page-level notice after the first H1 without disturbing its title."""
    lines = markdown.splitlines()
    for index, line in enumerate(lines):
        if line.startswith("# "):
            lines[index + 1:index + 1] = ["", notice, ""]
            return "\n".join(lines)
    return f"{notice}\n\n{markdown}"


def _relative_source_link(target_uri: str, source_uri: str) -> str:
    """Return a Markdown-source-relative link that MkDocs can validate."""
    source_directory = posixpath.dirname(source_uri) or "."
    return posixpath.relpath(target_uri, start=source_directory)


def on_page_markdown(markdown, *, page, config, **kwargs):
    """Keep audience/status cues and generated API behavior consistent site-wide."""
    source_uri = page.file.src_uri.replace("\\", "/")

    if source_uri.startswith("arkoi_language/"):
        page.meta.setdefault("search", {})["exclude"] = True
        # These pages are generated during the build, so their apparent Markdown
        # source paths do not exist in the repository.
        page.edit_url = None

        # MkDoxy emits machine-specific absolute paths in its page footer. Keep the
        # useful repository-relative path without leaking local or CI directories.
        config_path = Path(config.config_file_path).resolve()
        repository_root = config_path.parent.parent.as_posix()
        return markdown.replace(f"{repository_root}/", "")

    if (
        source_uri.startswith("language-specification/1.0.0/")
        and source_uri != "language-specification/1.0.0/index.md"
    ):
        compatibility_url = _relative_source_link(
            "getting-started/compatibility.md", source_uri
        )
        notice = (
            '!!! warning "Target Arkoi 1.0"\n\n'
            "    This page specifies the target language; the current compiler may "
            "not accept this syntax yet. See the "
            f"[compatibility guide]({compatibility_url})."
        )
        return _insert_after_title(markdown, notice)

    if (
        source_uri.startswith("getting-started/")
        and source_uri not in {
            "getting-started/overview.md",
            "getting-started/compatibility.md",
        }
    ):
        specification_url = _relative_source_link(
            "language-specification/1.0.0/index.md", source_uri
        )
        notice = (
            '!!! info "Current compiler"\n\n'
            "    Commands and syntax on this page describe the compiler available "
            "in this repository today. The "
            f"[Arkoi 1.0 reference]({specification_url}) describes the target language."
        )
        return _insert_after_title(markdown, notice)

    return markdown


def on_post_build(*, config, **kwargs):
    """Discard MkDoxy's unreferenced raw HTML/XML workspace from the public site."""
    generated_workspace = Path(config.site_dir) / "assets" / ".doxy"
    if generated_workspace.is_dir():
        shutil.rmtree(generated_workspace)
