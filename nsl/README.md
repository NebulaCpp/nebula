### Documentation de **wys.compiler** pour Nebula Simple Language (NSL)

---

#### Présentation

**wys.compiler** est un compilateur développé par wys pour le langage Nebula Simple Language (NSL), un dialecte de haut niveau simplifié destiné à faciliter la programmation assembleur avec NASM. Ce projet est conforme aux standards de Nebula et est conçu pour être simple et intuitif.

#### Informations Générales

- **Nom du Projet**: wys.compiler
- **Développeur Principal**: wys
- **Langage de Programmation**: C++
- **Langage Compilé**: Nebula Simple Language (NSL)
- **Normes**: Conforme aux standards de Nebula

#### Fonctionnalités

1. **Compilateur Single-Pass**: 
   - Traduit le code source NSL en code assembleur NASM en une seule passe.
   - Gère les fonctions, les boucles `while`, les conditions `if/elif`, les variables locales et globales.

2. **Gestion des Sections**:
   - Utilisation des sections `.text` alignée à 16 octets pour le code exécutable.
   - Utilisation des sections `.data` alignée à 8 octets pour les données statiques (variables globales).

3. **Support des Variables**:
   - Déclaration de variables avec `var` gérées comme variables locales ou globales.
   - Les variables locales sont réservées dans la section `.text`.
   - Les variables globales sont réservées dans la section `.data`.

#### Utilisation

1. **Syntaxe du Langage NSL**:

```plaintext
var globalVar

func main() {
    var localVar
    mov eax, 10
    .while (eax != 0)
        sub eax, 1
    .endwhile
    call my_function
}

func my_function() {
    var anotherLocalVar
    mov ebx, 5
    .if (ebx == 5)
        mov ecx, 10
    .elif (ebx == 3)
        mov ecx, 20
    .endif
}
```

2. **Compilation**:
   - Compiler le fichier source NSL avec **wys.compiler** en utilisant la commande :

   ```sh
   g++ -o wys.compiler wys_compiler.cpp
   ./wys.compiler input.src output.asm
   ```

3. **Assemblage et Liaison**:
   - **wys.compiler** génère automatiquement les commandes pour assembler et lier le fichier de sortie en fonction du système d'exploitation détecté (Windows, macOS, Linux).

#### Exemple de Fichier de Sortie `output.asm`

```asm
section .data align=8
globalVar resb 8

section .text align=16
global _start
main:
localVar resb 8
    mov eax, 10
L0_start:
    cmp eax, 0
    je L0_end
    sub eax, 1
    jmp L0_start
L0_end:
    call my_function

my_function:
anotherLocalVar resb 8
    mov ebx, 5
    cmp ebx, 5
    jne L1_else
    mov ecx, 10
    jmp L1_end
L1_else:
    cmp ebx, 3
    jne L1_end
    mov ecx, 20
L1_end:
```

#### Conclusion

**wys.compiler** pour Nebula Simple Language (NSL) simplifie le processus de programmation en assembleur en offrant une syntaxe claire et une gestion efficace des variables et des structures de contrôle. Il est conçu pour être facile à utiliser tout en respectant les standards de Nebula, facilitant ainsi le développement de logiciels bas-niveau.

---

Cette documentation couvre les aspects essentiels de **wys.compiler** pour NSL, fournissant aux développeurs les informations nécessaires pour utiliser et comprendre le compilateur dans le contexte du langage NSL et des standards de Nebula.
